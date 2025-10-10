/**
 * @file messages.c
 * @brief Functions for handling messages for peer-to-peer clock synchronization protocol.
 */

#include <endian.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <getopt.h>
#include <time.h>
#include <netdb.h>
#include <stdbool.h>
#include <poll.h>
#include <ifaddrs.h>


#include "err.h"
#include "common.h"
#include "messages.h"

/* Helper functions. */

/**
 * @brief Add new peer to the buffer.
 *
 * The buffer is assumed to be large enough to hold the new peer.
 *
 * @param buffer: pointer to the buffer
 * @param offset: pointer to the current offset in the buffer
 * @param peer: pointer to the peer to be added
 */
static void put_peer_in_buffer(uint8_t* buffer, ssize_t* offset,
                               const Node* peer) {
    // peer_address_length (1 byte, always 4 for IPv4).
    buffer[(*offset)++] = peer->peer_addr_len;

    // peer_address (4 bytes).
    memcpy(buffer + (*offset), &peer->peer_addr.s_addr, peer->peer_addr_len);
    (*offset) += peer->peer_addr_len;

    // peer_port (2 bytes, network byte order).
    uint16_t port_n = peer->peer_port;
    memcpy(buffer + (*offset), &port_n, PORT_SIZE);
    (*offset) += PORT_SIZE;
}

/**
 * @brief Read a 2-byte count from the buffer in network byte order and return it in host byte order.
 *
 * @param buffer: pointer to the buffer
 * @param offset: pointer to the current offset in the buffer
 * @return: the count in host byte order
 */
static uint16_t read_count(const uint8_t* buffer, size_t* offset) {
    uint16_t count;
    memcpy(&count, buffer + (*offset), COUNT_SIZE);
    (*offset) += COUNT_SIZE;

    return ntohs(count);
}

/**
 * @brief Read a 4-byte timestamp from the buffer in network byte order and return it in host byte order.
 *
 * @param buffer: pointer to the buffer
 * @param offset: the offset in the buffer
 *
 * @return: the timestamp in host byte order
 */
static uint64_t read_time(const uint8_t *buffer, size_t offset) {
    uint64_t timestamp;
    memcpy(&timestamp, buffer + offset, TIMESTAMP_SIZE);

    return be64toh(timestamp);
}

/**
 * @brief Read peer information from the buffer.
 *
 * @param buffer: pointer to the buffer
 * @param len: length of the buffer
 * @param ip: pointer to the IP address structure
 * @param offset: pointer to the current offset in the buffer
 * @param port: pointer to the port number
 *
 * @return: SUCCESS if successful, FAILURE otherwise
 */
static int read_peer_info(const uint8_t *buffer, size_t len, struct in_addr* ip,
                          size_t* offset, uint16_t* port) {
    uint8_t addr_len = buffer[(*offset)++];

    // Check if the address length is valid. According to the task,
    // it should be for IPv4.
    if (addr_len != IPV4_SIZE || (*offset) + addr_len + PORT_SIZE > len) {
      	return FAILURE;
    }

    // Read IP address.
    memcpy(&ip->s_addr, buffer + (*offset), IPV4_SIZE);
    (*offset) += IPV4_SIZE;

    // Read port.
    memcpy(port,  buffer + (*offset), PORT_SIZE);
    (*offset) += PORT_SIZE;

    return SUCCESS;
}

/**
 * @brief Checks if a given IPv4 address belongs to a local network interface.
 *
 * This function iterates over the system's network interfaces to determine
 * if the specified IPv4 address (`struct in_addr`) is one of the local addresses.
 *
 * @param addr The IPv4 address to check (in network byte order).
 *
 * @return 1 if the address is assigned to a local interface, 0 otherwise.
 */
static int is_local_ip(struct in_addr addr) {
    struct ifaddrs *ifaddr, *ifa;
    int result = 0;

    if (getifaddrs(&ifaddr) == -1) {
        error("getifaddrs");

        return 0;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        struct sockaddr_in *local_addr = (struct sockaddr_in *)ifa->ifa_addr;
        if (addr.s_addr == local_addr->sin_addr.s_addr) {
            result = 1;
            break;
        }
    }

    freeifaddrs(ifaddr);
    return result;
}

/**
 * @brief Check the validity of the peer to connect to.
 *
 * @param ip: IP address of the peer
 * @param port: port number of the peer
 * @param sender: pointer to the sender address structure
 * @param socket_fd: socket file descriptor
 *
 * @return: SUCCESS if the peer is valid, FAILURE otherwise
 */
static int check_if_valid_peer(struct in_addr ip, uint16_t port, const struct
                               sockaddr_in *sender, int socket_fd) {
  	// Check if the port is valid (greater than 0 - we don't have to check the
    // upper bound because of the uint16_t limit of size).
    if (port < MIN_PORT) {
        return FAILURE;
    }

    // Check if the sender is the same as the peer we are trying to connect to.
    if (ip.s_addr == sender->sin_addr.s_addr &&
        port == ntohs(sender->sin_port)) {
		return FAILURE;
    }

    // Access the socket to get the local address and port.
    struct sockaddr_in my_addr;
    socklen_t addr_len = sizeof(my_addr);

    if (getsockname(socket_fd, (struct sockaddr *)&my_addr, &addr_len) < 0) {
    	syserr("getsockname");
	}

    // If socket is bound to 0.0.0.0 (INADDR_ANY), compare sender IP to all local interfaces
    if (my_addr.sin_addr.s_addr == htonl(INADDR_ANY)) {
        if (is_local_ip(sender->sin_addr) && port == ntohs(my_addr.sin_port)) {
            return FAILURE;
        }
    } else {
        // Otherwise, just compare with the bound IP
        if (sender->sin_addr.s_addr == my_addr.sin_addr.s_addr &&
            port == ntohs(my_addr.sin_port)) {
            return FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * @brief Send a CONNECT message to the peer.
 *
 * @param ip: IP address of the peer
 * @param port: port number of the peer
 */
static void send_connect(int socket_fd, struct in_addr ip, uint16_t port) {
    // Create a CONNECT message.
    uint8_t buffer[CONNECT_SIZE];
    buffer[0] = MSG_CONNECT;

    struct sockaddr_in send_address = create_address(ip, port);

    if (sendto(socket_fd, buffer, CONNECT_SIZE, FLAGS,
       (struct sockaddr *)&send_address, sizeof(send_address)) < CONNECT_SIZE) {
        error("sendto (CONNECT)");
    }
}

/**
 * @brief Check if the synchronization level of the peer is valid.
 *
 * Level should be:
 * at least one lower than the current synchronization level of node (if we
 * are synchronized with this peer).
 * or at least two lower than the current synchronization level of node (if we
 * are not synchronized with this peer).
 *
 * @param my_info: my information structure
 * @param synchronized_peer: index of the peer we are synchronized with
 * @param peer_idx: index of the peer that sent SYNC_START message
 * @param sender_sync: synchronization level of the peer that sent SYNC_START
 *
 * @return: true if the synchronization level is valid, false otherwise
 */
static bool validate_sync_level(MyInfo *my_info, int synchronized_peer,
                                int peer_idx, uint8_t sender_sync) {
	if (my_info->synchronization_level < SYNC_NOT_SYNCED
      && my_info->synchronization_level != SYNC_LEADER
      && synchronized_peer == peer_idx) {
        if (my_info->synchronization_level <= sender_sync) {
          	my_info->synchronization_level = SYNC_NOT_SYNCED;
            return false;
        }
    } else if (my_info->synchronization_level <= sender_sync + 1) {
        return false;
    }

    return true;
}

/*
 * @brief Check if the sender of SYNC_START message is valid.
 *
 * @param peer_idx: index of the peer that sent SYNC_START message
 * @param my_info: my information structure
 * @param peer_info: list of connected peers
 * @param sender_sync: synchronization level of the peer that sent SYNC_START
 *
 * @return: true if the sender is valid, false otherwise
 */
static bool is_sync_start_valid(int peer_idx, MyInfo *my_info,
                                PeerInfo* peer_info, uint8_t sender_sync) {
    if (sender_sync >= MAX_SYNCED) {

        return false; // Level of synchronization is too big.
    }

    if (peer_idx == -1) {

        return false; // Sender unknown.
    }

    return validate_sync_level(my_info, peer_info->synchronized_peer, peer_idx,
                               sender_sync);
}


/* Functions for joining the network */

void send_hello(int socket_fd, const struct sockaddr_in *peer_addr) {
    uint8_t buffer[HELLO_SIZE];
    buffer[0] = MSG_HELLO;

    if (sendto(socket_fd, buffer, HELLO_SIZE, FLAGS, (struct sockaddr *)peer_addr,
               sizeof(*peer_addr)) != HELLO_SIZE) {
        error("sendto (HELLO)");
    }
}

void send_hello_reply(int socket_fd, const struct sockaddr_in *peer_addr,
                      PeerInfo* info, uint8_t *const buffer) {
    ssize_t offset = 0;

    buffer[offset++] = MSG_HELLO_REPLY;

    // Add 2-byte count (network order).
    uint16_t count_n = htons(info->peer_count);
    memcpy(buffer + offset, &count_n, COUNT_SIZE);
    offset += COUNT_SIZE;

    // Add peers to the buffer.
    for (size_t i = 0; i < info->peer_count; ++i) {
        if (offset + 1 + info->connected_nodes[i].peer_addr_len + PORT_SIZE
          > BUF_SIZE) {
            error_msg(buffer, offset);

            return;
        }

        put_peer_in_buffer(buffer, &offset, &info->connected_nodes[i]);
    }

    if (sendto(socket_fd, buffer, offset, FLAGS,
       (struct sockaddr *)peer_addr, sizeof(*peer_addr)) != offset) {
        error("sendto (HELLO_REPLY)");
    }

    // Add the sender to the peer list.
    add_or_update_peer(peer_addr, info);
}

void receive_hello_reply(const uint8_t *buffer, size_t len, const struct sockaddr_in
                         *sender, int socket_fd, PeerInfo* peer_info,
                         PeerInfo* waiting_to_connect) {
    size_t offset = MSG_SIZE; // Skip the message type byte.

    // Check if the length of the message is valid (at least 3 bytes: message
    // type - 1 byte and count - 2 bytes).
    if (len < MIN_HELLO_REPLY_SIZE) {
        error_msg(buffer, len);

        return;
    }

    uint16_t count = read_count(buffer, &offset);

    for (uint16_t i = 0; i < count; ++i) {
        // Check if the offset is within bounds if it isn't it means that
        // the message was corupted.
      	if (offset >= len) {
            error_msg(buffer, len);
      	    waiting_to_connect->peer_count = 0; // Reset the waiting list.

            return;
        }

        struct in_addr ip;
        uint16_t port;

        // Read and validate peer information from the buffer.
        if (read_peer_info(buffer, len, &ip, &offset, &port) == FAILURE) {
            error_msg(buffer, len);
            waiting_to_connect->peer_count = 0; // Reset the waiting list.

            return;
        }

        if (check_if_valid_peer(ip, port, sender, socket_fd) == FAILURE) {
            error_msg(buffer, len);
            waiting_to_connect->peer_count = 0; // Reset the waiting list.

            return;
        }

        struct sockaddr_in peer_addr = create_address(ip, port);
        add_or_update_peer(&peer_addr, waiting_to_connect);
    }

    // Send connect, only if whole message is valid.
    for (uint16_t i = 0; i < waiting_to_connect->peer_count; ++i) {
        send_connect(socket_fd, waiting_to_connect->connected_nodes[i].peer_addr,
                   waiting_to_connect->connected_nodes[i].peer_port);
    }

    // Add the sender to the peer list.
    add_or_update_peer(sender, peer_info);

    return;
}

void receive_connect(struct sockaddr_in *sender, PeerInfo *peer_info,
                     int socket_fd) {
    // Add the sender to the peer list.
    add_or_update_peer(sender, peer_info);

    // Send ACK_CONNECT.
    uint8_t buffer[ACK_CONNECT_SIZE];
    buffer[0] = MSG_ACK_CONNECT;

    if (sendto(socket_fd, buffer, ACK_CONNECT_SIZE, FLAGS, (struct sockaddr *)sender,
               sizeof(*sender)) != ACK_CONNECT_SIZE) {
        error("sendto (ACK_CONNECT)");
    }
}

void receive_ack_connect(const struct sockaddr_in *sender, PeerInfo *info) {
    add_or_update_peer(sender, info);
}


/* Functions for synchronization */

void send_sync_start(MyInfo *my_info, PeerInfo *peer_info,
                     SynchInfo* synch_info) {
    uint8_t msg[SYNC_START_SIZE]; // 1 bajt msg + 1 sync + 8 timestamp
    msg[0] = MSG_SYNC_START;

    msg[1] = my_info->synchronization_level;

    for (int i = 0; i < peer_info->peer_count; ++i) {
        struct sockaddr_in send_address;
        send_address = create_address(peer_info->connected_nodes[i].peer_addr,
                                      peer_info->connected_nodes[i].peer_port);

        uint64_t timestamp = get_time_now(my_info->clock_start,
                                      my_info->time_offset);

        uint64_t net_ts = htobe64(timestamp);
        memcpy(&msg[2], &net_ts, TIMESTAMP_SIZE);

        if (sendto(my_info->socket_fd, &msg, SYNC_START_SIZE, FLAGS,
           (struct sockaddr *)&send_address, sizeof(send_address)) != SYNC_START_SIZE) {
            error("sendto (SYNC_START)");
        }

        peer_info->connected_nodes[i].time = get_local_time_ms();

    }

    synch_info->last_sync_time = get_local_time_ms();
}

void receive_sync_start(const uint8_t *buffer, const struct sockaddr_in *sender,
    MyInfo *my_info, SynchInfo* synch_info, PeerInfo* peer_info) {
    // Save the time (T2) of receiving SYNC_START message.
    uint64_t time = get_local_time_ms() - my_info->clock_start;

    if (synch_info->is_currently_syncing) {
        if (get_local_time_ms() - synch_info->request_time < SYNC_INTERVAL) {
            error_msg(buffer, SYNC_START_SIZE);

            return;
        }
    }

    uint8_t sender_sync = buffer[MSG_SIZE]; // Skip the message type byte.
    uint64_t recv_ts = read_time(buffer, MSG_SIZE + 1); // Skip the sync level.

    int peer_idx = find_peer_index(sender, peer_info);

    if (!is_sync_start_valid(peer_idx, my_info, peer_info, sender_sync)) {
        error_msg(buffer, SYNC_START_SIZE);

        return;
    }

    synch_info->is_currently_syncing = true;
    synch_info->current_synch_level_of_peer = sender_sync;
    peer_info->currently_synchronized_peer = peer_idx;
    synch_info->new_offset = time; // T2
    synch_info->new_offset -= recv_ts; // - T1

    uint8_t msg[DELAY_REQUEST_SIZE];
    msg[0] = MSG_DELAY_REQUEST;

    if (sendto(my_info->socket_fd, msg, MSG_SIZE, FLAGS,
               (struct sockaddr *)sender, sizeof(*sender)) != MSG_SIZE) {
        error("sendto (DELAY_REQUEST)");
    }

    // Add the time of sending the request (T3) to the new offset.
    synch_info->new_offset += get_local_time_ms() - my_info->clock_start;
    // Save the time of sending the request.
    synch_info->request_time = get_local_time_ms();
}

void send_delay_response(struct sockaddr_in *receiver, const MyInfo* info) {
    uint8_t buffer[DELAY_RESPONSE_SIZE]; // 1 bajt msg + 1 sync + 8 timestamp

    buffer[0] = MSG_DELAY_RESPONSE;
    buffer[1] = info->synchronization_level;

    uint64_t timestamp = get_time_now(info->clock_start, info->time_offset);

    uint64_t net_ts = htobe64(timestamp);
    memcpy(&buffer[2], &net_ts, 8);

    if (sendto(info->socket_fd, buffer, DELAY_RESPONSE_SIZE, FLAGS,
               (struct sockaddr *)receiver, sizeof(*receiver)) != DELAY_RESPONSE_SIZE) {
        error("sendto (DELAY_RESPONSE)");
    }
}

void receive_delay_response(const uint8_t *buffer, MyInfo* my_info,
                            SynchInfo* synch_info, PeerInfo* peer_info) {
    if (synch_info->is_currently_syncing == false) {
        error_msg(buffer, DELAY_RESPONSE_SIZE);

        return;
    }

    if (get_local_time_ms() - synch_info->request_time > SYNC_INTERVAL) {
        error_msg(buffer, DELAY_RESPONSE_SIZE);
        synch_info->is_currently_syncing = false;

        return;
    }

    uint8_t sender_sync = buffer[MSG_SIZE]; // Skip the message type byte.

    // Check if the sender's synchronization level is still valid (the same as
    // it was before).
    if (synch_info->current_synch_level_of_peer != sender_sync) {
        error_msg(buffer, DELAY_RESPONSE_SIZE);
        synch_info->is_currently_syncing = false;

        return;
    }

    // Skip the message type and sync level bytes.
    uint64_t recv_ts = read_time(buffer, MSG_SIZE + 1);

    synch_info->new_offset -= recv_ts;
    my_info->time_offset = synch_info->new_offset / 2;
    synch_info->new_offset = 0;

    my_info->synchronization_level = sender_sync + 1;
    synch_info->synch_level_of_peer = sender_sync;
    peer_info->synchronized_peer = peer_info->currently_synchronized_peer;
    synch_info->is_currently_syncing = false;
    synch_info->waiting_time = get_local_time_ms();
}

/* Function for leader election */

void receive_leader(const uint8_t *buffer, ssize_t len, MyInfo* info,
                    SynchInfo* synch_info) {
  	uint8_t new_sync = buffer[MSG_SIZE]; // Skip the message type byte.

    if (new_sync == SYNC_LEADER) {
        info->synchronization_level = SYNC_LEADER;
		info->time_offset = 0;
        // Shorten the time for waiting to send sync_start to 2s.
        synch_info->last_sync_time = get_local_time_ms() - LEADER_SHORT_TIMEOUT;
    } else if (new_sync == SYNC_NOT_SYNCED) {
        if (info->synchronization_level == SYNC_LEADER) {
            info->synchronization_level = SYNC_NOT_SYNCED;
        } else {
            error_msg(buffer, len);
        }
    } else {
        error_msg(buffer, len);
    }
}

/* Functions for accessing the time */

void send_time_response(struct sockaddr_in *receiver, const MyInfo* info) {
    uint8_t buffer[TIME_SIZE]; // 1 bajt msg + 1 sync + 8 timestamp

    buffer[0] = MSG_TIME;
    buffer[1] = info->synchronization_level;

    uint64_t timestamp = get_local_time_ms() - info->clock_start;

    if (info->synchronization_level != SYNC_NOT_SYNCED) {
        timestamp -= info->time_offset;
    }

    uint64_t net_ts = htobe64(timestamp);
    memcpy(&buffer[2], &net_ts, TIMESTAMP_SIZE);

    if (sendto(info->socket_fd, buffer, TIME_SIZE, FLAGS, (struct sockaddr *)receiver,
           sizeof(*receiver)) != TIME_SIZE) {
        error("sendto (TIME)");
    }
}