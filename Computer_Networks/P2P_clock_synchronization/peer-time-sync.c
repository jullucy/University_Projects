/**
 * @file peer-time-sync.c
 * @brief Peer-to-peer clock synchronization program.
 *
 * This program synchronizes clocks across a distributed peer-to-peer network
 * using UDP messaging. It handles node connection, synchronization, and
 * communication protocols.
 */

#include <endian.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <getopt.h>
#include <time.h>
#include <netdb.h>
#include <stdbool.h>
#include <poll.h>
#include <signal.h>

#include "err.h"
#include "common.h"
#include "messages.h"

/** Maximum number of peers that can be connected. */
#define MAX_PEERS 65535
/** Time interval between SYNC_START messages in milliseconds. */
#define SYNC_START_INTERVAL 5000 // 5 seconds
/** Time interval after which node becomes not synchronized, in milliseconds. */
#define WAITING_INTERVAL 20000 // 20 seconds
/** Time interval for pool */
#define POLL_INTERVAL 2000 // 2 seconds
/** Default port number if none is specified. */
#define DEFAULT_PORT 0

/** Flag indicating whether the program should terminate. */
static bool finish = false;

/**
 * @brief Structure storing connection configuration.
 */
typedef struct {
    char *my_address;    ///< Local address to bind.
    int port;            ///< Local port number.
    char *peer_address;  ///< Address of initial peer to connect to.
    int peer_port;       ///< Port of initial peer to connect to.
} Config;


/* Initialization of structures */

/**
 * @brief Initialize MyInfo structure.
 * @return Initialized MyInfo structure.
 */
static MyInfo my_info_init() {
    MyInfo my_info;
    my_info.clock_start = get_local_time_ms();
    my_info.synchronization_level = SYNC_NOT_SYNCED;
    my_info.time_offset = 0;
    my_info.socket_fd = -1;

    return my_info;
}

/**
 * @brief Initialize PeerInfo structure.
 * @return Initialized PeerInfo structure.
 */
static PeerInfo peer_info_init() {
    PeerInfo peer_info;
    peer_info.connected_nodes = NULL;
    peer_info.connected_nodes_size = 0;
    peer_info.peer_count = 0;
    peer_info.synchronized_peer = 0;
    peer_info.currently_synchronized_peer = 0;

    return peer_info;
}

/**
 * @brief Initialize SynchInfo structure.
 * @return Initialized SynchInfo structure.
 */
static SynchInfo synch_info_init() {
    SynchInfo synch_info;
    synch_info.last_sync_time = 0;
    synch_info.is_currently_syncing = false;
    synch_info.new_offset = 0;
    synch_info.synch_level_of_peer = 0;
    synch_info.request_time = 0;
    synch_info.waiting_time = 0;
    synch_info.current_synch_level_of_peer = 0;

    return synch_info;
}

/**
 * @brief Initialize Config structure.
 * @return Initialized Config structure.
 */
static Config config_init() {
    Config config;
    config.my_address = NULL;
    config.port = DEFAULT_PORT;
    config.peer_address = NULL;
    config.peer_port = DEFAULT_PORT;

    return config;
}

/**
 * @brief Signal handler for graceful termination (SIGINT).
 */
static void catch_int() {
    finish = true;
}

/* Helper functions */

/**
 * @brief Parse command-line parameters.
 *
 * Recognized options:
 * - `-b`: Bind address.
 * - `-p`: Bind port.
 * - `-a`: Peer address to connect to.
 * - `-r`: Peer port to connect to.
 *
 * @param opt Current option character from getopt.
 * @param config Configuration structure to populate.
 */
static void parse_parameters(const int opt, Config *config) {
    switch (opt) {
        case 'b':
            config->my_address = optarg;

            break;
        case 'p':
            config->port = read_port(optarg);

            if (config->port < DEFAULT_PORT || config->port > MAX_PORT) {
                fatal( "Invalid port number: %d\n", config->port);
            }

            break;
        case 'a':
            config->peer_address = optarg;

            break;
        case 'r':
            config->peer_port = read_port(optarg);

            if (config->peer_port < MIN_PORT || config->peer_port > MAX_PORT) {
                fatal("Invalid peer port: %d\n", config->peer_port);
            }

            break;
        default:
            fatal("Invalid option: %c\n", opt);
    }
}

/**
 * @brief Validate the consistency of parsed command-line parameters.
 *
 * Ensures that both peer address and peer port are specified together.
 *
 * @param config Pointer to the Config structure.
 */
static void validate_parameters(const Config *config) {
    if ((config->peer_address != NULL && config->peer_port == DEFAULT_PORT) ||
        (config->peer_address == NULL && config->peer_port != DEFAULT_PORT)) {
        fatal("Both -a and -r must be specified together.\n");
        }
}

/**
 * @brief Check if the peer list has reached the maximum allowed size.
 *
 * @param peer_info Pointer to the PeerInfo structure.
 * @return true if peer list is full, false otherwise.
 */
static bool is_max_peers(const PeerInfo *peer_info) {
    if (peer_info->peer_count >= MAX_PEERS) {

        return true;
    }

    return false;
}

/**
 * @brief Validate HELLO_REPLY message sender.
 *
 * @param config Pointer to the Config structure.
 * @param sender Sender address.
 * @return true if sender matches the expected peer, false otherwise.
 */
static bool is_hello_valid(const Config *config,
    const struct sockaddr_in *sender) {
    if (config->peer_address == NULL) {
        return false;
    }

    const struct sockaddr_in peer_address = get_peer_address(
        config->peer_address, config->peer_port);

    if (peer_address.sin_addr.s_addr == sender->sin_addr.s_addr &&
        peer_address.sin_port == sender->sin_port) {
        return true;
    }

    return false;
}

/**
 * @brief Validate if the sender is a peer we sent CONNECT to.
 *
 * If the sender is on the list, remove it.
 *
 * @param waiting_to_connect List of peers waiting to connect.
 * @param sender Sender address.
 * @return true if sender is in the waiting list, false otherwise.
 */
static bool is_ask_connect_valid(PeerInfo *waiting_to_connect,
    const struct sockaddr_in *sender) {
    const int peer_idx = find_peer_index(sender, waiting_to_connect);

    if (peer_idx == -1) {

        return false; // Sender unknown.
    }

    delete_peer(peer_idx, waiting_to_connect);

    return true;
}

/**
 * @brief Validate if DELAY_REQUEST is from a known and recently contacted peer.
 *
 * @param peer_info Pointer to the PeerInfo structure.
 * @param sender Sender address.
 * @return true if valid, false otherwise.
 */
static bool is_delay_request_valid(const PeerInfo *peer_info,
    const struct sockaddr_in *sender) {
    const int peer_idx = find_peer_index(sender, peer_info);

    if (peer_idx == -1) {

        return false; // Sender unknown.
    }

    const uint64_t time = peer_info->connected_nodes[peer_idx].time;

    if (time == 0 || get_local_time_ms() - time > SYNC_INTERVAL) {
        return false;
    }

    return true;
}

/**
 * @brief Check if we should send a SYNC_START message.
 *
 * Following conditions must be met:
 * - Synchronization level is less than 254.
 * - At least SYNC_START_INTERVAL milliseconds have passed since the last SYNC_START.
 *
 * @param my_info Pointer to the MyInfo structure.
 * @param synch_info Pointer to the SynchInfo structure.
 * @return true if SYNC_START should be sent, false otherwise.
 */
static bool should_send_synchronize(const MyInfo *my_info,
    const SynchInfo *synch_info) {
    if (my_info->synchronization_level < MAX_SYNCED &&
        get_time_now(synch_info->last_sync_time, 0) > SYNC_START_INTERVAL) {

        return true;
    }

    return false;
}


/**
 * @brief Update synchronization status based on timeout.
 *
 * If more than WAITING_INTERVAL milliseconds have passed since the last
 * the node changes its synchronization level to SYNC_NOT_SYNCED.
 *
 * @param my_info Pointer to the MyInfo structure.
 * @param synch_info Pointer to the SynchInfo structure.
 */
static void is_still_synchronized(MyInfo *my_info, const SynchInfo *synch_info) {
    if (my_info->synchronization_level != SYNC_NOT_SYNCED &&
        my_info->synchronization_level != SYNC_LEADER &&
        get_time_now(synch_info->waiting_time, 0) > WAITING_INTERVAL) {
        my_info->synchronization_level = SYNC_NOT_SYNCED;
    }
}

/**
 * @brief Validate if DELAY_RESPONSE came from the currently synchronized peer.
 *
 * @param peer_info Pointer to the PeerInfo structure.
 * @param sender Sender address.
 * @return true if valid, false otherwise.
 */
static bool is_delay_response_valid(const PeerInfo *peer_info,
    const struct sockaddr_in *sender) {
    const int peer_idx = find_peer_index(sender, peer_info);

    if (peer_idx != peer_info->currently_synchronized_peer) {

        return false; // Not syncing with this peer.
    }

    return true;
}

/**
 * @brief Clean up resources and close the socket.
 *
 * @param my_info Pointer to the MyInfo structure.
 * @param peer_info Pointer to the PeerInfo structure.
 * @param waiting_to_connect Pointer to the waiting PeerInfo structure.
 * @param buffer Allocated message buffer.
 */
static void clean_data(const MyInfo *my_info, const PeerInfo *peer_info,
    const PeerInfo *waiting_to_connect, uint8_t *buffer) {
    free(peer_info->connected_nodes);
    free(waiting_to_connect->connected_nodes);
    free(buffer);
    close(my_info->socket_fd);
}

/**
 * @brief Handle a received network message.
 *
 * Decodes message type and dispatches appropriate handler.
 * Accepted message types:
 * - HELLO - message from peer who just joined the network.
 * - HELLO_REPLY - message from peer we sent HELLO to.
 * - CONNECT - message from peer who wants to connect with us, it is sent by
 *   node which newly joined the network, to every node sent to it in
 *   HELLO_REPLY message.
 * - ACK_CONNECT - answer to CONNECT message, it confirms the connection.
 * - SYNC_START - message to start synchronization process.
 * - DELAY_REQUEST - message to request time from the peer we are syncing with
 *   (essential for counting the delay).
 * - DELAY_RESPONSE - message to respond to DELAY_REQUEST message.
 * - LEADER - message to inform about the leader of the network (it can either
 *   give a node rights of a leader or take them from it).
 * - GET_TIME - message to request time from node.
 *
 * @param buffer Pointer to received message buffer.
 * @param len Length of the received message.
 * @param sender Pointer to sender address structure.
 * @param my_info Pointer to the MyInfo structure.
 * @param config Pointer to the Config structure.
 * @param peer_info Pointer to the PeerInfo structure.
 * @param synch_info Pointer to the SynchInfo structure.
 * @param waiting_to_connect Pointer to the PeerInfo structure holding peers waiting to connect.
 */
static void handle_message(uint8_t *const buffer, const ssize_t len,
    struct sockaddr_in *sender, MyInfo *my_info, const Config *config,
    PeerInfo *peer_info, SynchInfo *synch_info, PeerInfo *waiting_to_connect) {
    if (len < MSG_SIZE) return;

    const uint8_t message_type = buffer[0];

    switch (message_type) {
        case MSG_HELLO:
            if (len == HELLO_SIZE && !is_max_peers(peer_info)) {
                send_hello_reply(my_info->socket_fd, sender, peer_info, buffer);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_HELLO_REPLY:
            if (is_hello_valid(config, sender) && !is_max_peers(peer_info)) {
                receive_hello_reply(buffer, len, sender, my_info->socket_fd,
                    peer_info, waiting_to_connect);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_CONNECT:
            if (len == CONNECT_SIZE && !is_max_peers(peer_info)) {
                receive_connect(sender, peer_info, my_info->socket_fd);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_ACK_CONNECT:
            if (len == ACK_CONNECT_SIZE && !is_max_peers(peer_info) &&
                is_ask_connect_valid(waiting_to_connect, sender)) {
                receive_ack_connect(sender, peer_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_SYNC_START:
            if (len == SYNC_START_SIZE) {
                receive_sync_start(buffer, sender, my_info, synch_info, peer_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_DELAY_REQUEST:
            if (len == DELAY_REQUEST_SIZE && is_delay_request_valid(peer_info, sender)) {
                send_delay_response(sender, my_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_DELAY_RESPONSE:
            if (len == DELAY_RESPONSE_SIZE && is_delay_response_valid(peer_info, sender)) {
                receive_delay_response(buffer, my_info, synch_info, peer_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_LEADER:
            if (len == LEADER_SIZE) {
                receive_leader(buffer, len, my_info, synch_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        case MSG_GET_TIME:
            if (len == GET_TIME_SIZE) {
                send_time_response(sender, my_info);
            } else {
                error_msg(buffer, len);
            }

            break;
        default:
            error_msg(buffer, len);

            break;
    }
}

/**
 * @brief Receive and process a message from the socket.
 *
 * @param my_info Pointer to the MyInfo structure.
 * @param config Pointer to the Config structure.
 * @param buffer Pointer to buffer for incoming data.
 * @param peer_info Pointer to PeerInfo structure.
 * @param synch_info Pointer to SynchInfo structure.
 * @param waiting_to_connect Pointer to PeerInfo for pending connections.
 */
static void receive_message(MyInfo* my_info, const Config *config,
    uint8_t *buffer, PeerInfo *peer_info, SynchInfo *synch_info,
    PeerInfo *waiting_to_connect) {
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    const ssize_t len = recvfrom(my_info->socket_fd, buffer, BUF_SIZE, FLAGS,
                           (struct sockaddr *) &sender, &sender_len);

    if (len > 0) {
        handle_message(buffer, len, &sender, my_info, config, peer_info,
            synch_info, waiting_to_connect);
    } else {
        error_msg(buffer, len);
    }
}

/* Main program entry point */

/**
 * @brief Program entry point.
 *
 * Initializes networking, sets up signal handlers, handles peer discovery,
 * and enters the main communication loop.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on successful termination, 1 otherwise.
 */
int main(int argc, char *argv[]) {
    // Initialize structures.
    MyInfo my_info = my_info_init();
    PeerInfo peer_info = peer_info_init();
    PeerInfo waiting_to_connect = peer_info_init();
    SynchInfo synch_info = synch_info_init();
    Config config = config_init();

    install_signal_handler(SIGINT, catch_int, SA_RESTART);

    int opt;

    // Read command line arguments.
    while ((opt = getopt(argc, argv, "b:p:a:r:")) != -1) {
        parse_parameters(opt, &config);
    }

    validate_parameters(&config);

    // Create a socket.
    my_info.socket_fd = create_socket();

    // Bind the socket to the specified address and port.
    bind_socket(my_info.socket_fd, config.port, config.my_address);

    // Send HELLO message to the peer if specified.
    if (config.peer_address != NULL) {
        struct sockaddr_in peer_address = get_peer_address(config.peer_address,
            config.peer_port);
        send_hello(my_info.socket_fd, &peer_address);
    }

    uint8_t* buffer = malloc(sizeof(uint8_t) * BUF_SIZE);
    if (buffer == NULL) {
        syserr("malloc");
    }
    do {
        struct pollfd fds[1];
        fds[0].fd = my_info.socket_fd;
        fds[0].events = POLLIN;

        // Poll for a short timeout so we can check SYNC_START periodically.
        const int poll_count = poll(fds, 1, POLL_INTERVAL);

        is_still_synchronized(&my_info, &synch_info);

        if (should_send_synchronize(&my_info, &synch_info)) {
            send_sync_start(&my_info, &peer_info, &synch_info);
        }

        if (poll_count > 0) {
            receive_message(&my_info, &config, buffer, &peer_info, &synch_info,
                &waiting_to_connect);
        } else if (poll_count < 0) {
            clean_data(&my_info, &peer_info, &waiting_to_connect, buffer);
            syserr("poll");
        }
    } while (!finish);

    clean_data(&my_info, &peer_info, &waiting_to_connect, buffer);

    return EXIT_SUCCESS;
}

