/**
 * @file common.c
 * @brief This file contains common functions used in the project.
 *
 * It includes functions for reading port numbers, getting peer addresses,
 * creating sockets, managing connected nodes, and installing signal handlers.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>

#include "err.h"
#include "common.h"

uint16_t read_port(const char *string) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);

    if (errno != 0 || *endptr != 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }

    return (uint16_t) port;
}

struct sockaddr_in get_peer_address(const char *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;


    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);

    if (errcode != 0) {
        fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;   // IPv4
    send_address.sin_addr.s_addr =       // IP address
            (((struct sockaddr_in *) address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

struct sockaddr_in create_address(struct in_addr ip, uint16_t port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;   // IPv4
    address.sin_addr.s_addr = ip.s_addr;
    address.sin_port = port; // port should already be in network byte order.

    return address;
}

int create_socket() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    return socket_fd;
}

void bind_socket(int socket_fd, uint16_t port, char* bind_address) {
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);

    if (bind_address) {
        if (inet_pton(AF_INET, bind_address, &server_address.sin_addr) <= 0) {
            syserr("inet_pton (bind_address)");
        }
    } else {
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    if (bind(socket_fd, (struct sockaddr *) &server_address,
             (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
}

uint64_t get_local_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

uint64_t get_time_now(uint64_t start_time, uint64_t offset) {
    uint64_t time_now = get_local_time_ms();
    time_now -= start_time;
    time_now -= offset;

    return time_now;
}


/* Nodes array menagement */

/**
 * @brief Compare two peers based on their address and port.
 *
 * This function is used for sorting and searching in the array of peers.
 *
 * @param a: pointer to the first peer
 * @param b: pointer to the second peer
 * @return: negative if a < b, positive if a > b, 0 if equal
 */
static int compare_peers(const struct sockaddr_in *a, const Node *b) {
    if (a->sin_addr.s_addr != b->peer_addr.s_addr)
        return (a->sin_addr.s_addr < b->peer_addr.s_addr) ? -1 : 1;
    if (a->sin_port != b->peer_port)
        return (a->sin_port < b->peer_port) ? -1 : 1;

    return 0;
}

Node create_node(struct in_addr peer_address, uint16_t peer_port) {
    Node node;
    node.peer_addr.s_addr = peer_address.s_addr;
    node.peer_port = peer_port;
    node.peer_addr_len = sizeof(node.peer_addr);
    node.time = 0;

    return node;
}

int find_peer_index(const struct sockaddr_in *peer_addr, const PeerInfo *info) {
  	int low = 0;
    int high = info->peer_count - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = compare_peers(peer_addr, &info->connected_nodes[mid]);

        if (cmp == 0)
            return mid;
        else if (cmp < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }

    return -low - 1;
}

void add_or_update_peer(const struct sockaddr_in *new_peer, PeerInfo *info) {
    int index = find_peer_index(new_peer, info);

    if (index >= 0) {
      	Node new_node = create_node(new_peer->sin_addr, new_peer->sin_port);
        info->connected_nodes[index] = new_node; // Update

        return;
    }

    // Insert at ~index
    int insert_pos = -index - 1;

    if (info->peer_count == info->connected_nodes_size) {
        size_t new_capacity = info->connected_nodes_size == 0 ? 8 : info->connected_nodes_size * 2;
        Node *new_list = realloc(info->connected_nodes,
                                 new_capacity * sizeof(Node));

        if (!new_list)
            syserr("out of memory");

        info->connected_nodes = new_list;
        info->connected_nodes_size = new_capacity;
    }

    // Shift right to make space
    for (int i = info->peer_count; i > insert_pos; --i) {
        info->connected_nodes[i] = info->connected_nodes[i - 1];
    }

    // Move synchronized and currently synchronized peers.
    if (info->synchronized_peer >= insert_pos) {
        info->synchronized_peer++;
    }
    if (info->currently_synchronized_peer >= insert_pos) {
        info->currently_synchronized_peer++;
    }

    Node new_node = create_node(new_peer->sin_addr, new_peer->sin_port);
    info->connected_nodes[insert_pos] = new_node;
    info->peer_count++;
}

void delete_peer(int peer_index, PeerInfo* info) {
    if (peer_index < 0 || peer_index >= info->peer_count) {
        return; // Invalid index
    }

    // Shift peers to fill the gap
    for (int i = peer_index; i < info->peer_count - 1; ++i) {
        info->connected_nodes[i] = info->connected_nodes[i + 1];
    }

    // Move synchronized and currently synchronized peers.
    if (info->synchronized_peer > peer_index) {
        info->synchronized_peer--;
    }
    if (info->currently_synchronized_peer > peer_index) {
        info->currently_synchronized_peer--;
    }

    info->peer_count--;
}

/* Signal handling */

void install_signal_handler(int signal, void (*handler)(), int flags) {
    struct sigaction action;
    sigset_t block_mask;

    sigemptyset(&block_mask);
    action.sa_handler = handler;
    action.sa_mask = block_mask;
    action.sa_flags = flags;

    if (sigaction(signal, &action, NULL) < 0) {
        syserr("sigaction");
    }
}
