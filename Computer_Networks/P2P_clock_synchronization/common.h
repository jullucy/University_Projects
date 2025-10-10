/**
 * @file common.h
 * @brief Common structures and functions for peer-to-peer clock synchronization.
 *
 * This file defines data structures and declares functions for networking,
 * peer management, timing, and signal handling.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

/** Maximum number of peers allowed. */
#define MAX_PEERS 65535

/**
 * @struct Node
 * @brief Represents a connected peer node.
 */
typedef struct {
    uint8_t peer_addr_len;               /**< Length of the peer address. */
    struct in_addr peer_addr;            /**< IP address of the peer. */
    uint16_t peer_port;                  /**< Port number of the peer. */
    uint64_t time;                       /**< Last time a SYNC_START was sent to this peer. */
} Node;

/**
 * @struct MyInfo
 * @brief Holds information about the local node.
 */
typedef struct {
    uint64_t clock_start;                /**< Milliseconds since the node started. */
    uint8_t synchronization_level;       /**< Current synchronization level. */
    int64_t time_offset;                 /**< Time offset for synchronization. */
    int socket_fd;                       /**< Socket file descriptor. */
} MyInfo;

/**
 * @struct PeerInfo
 * @brief Manages a dynamic list of connected peer nodes.
 */
typedef struct {
    Node* connected_nodes;               /**< Dynamic array of connected nodes. */
    size_t connected_nodes_size;         /**< Size of the connected_nodes array. */
    uint16_t peer_count;                 /**< Current number of connected peers. */
    int synchronized_peer;               /**< Index of the peer we are synchronized with. */
    int currently_synchronized_peer;     /**< Index of the peer we are currently syncing with. */
} PeerInfo;

/**
 * @struct SynchInfo
 * @brief Holds synchronization session information.
 */
typedef struct {
    uint64_t last_sync_time;             /**< Last time a SYNCHRONIZED message was sent. */
    uint64_t waiting_time;               /**< Waiting time for a response. */
    int64_t new_offset;                  /**< New computed time offset. */
    uint8_t synch_level_of_peer;         /**< Synchronization level of the peer we are syncing with. */
    uint8_t current_synch_level_of_peer; /**< Current synchronization level of the peer. */
    uint64_t request_time;               /**< Time when a DELAY_REQUEST was sent. */
    bool is_currently_syncing;           /**< Whether a synchronization is ongoing. */
} SynchInfo;

/* ===========================
 * Function declarations
 * =========================== */

/**
 * @brief Parses a string into a port number.
 *
 * @param string String representation of the port number.
 * @return Parsed port number.
 */
uint16_t read_port(const char *string);

/**
 * @brief Resolves a hostname or IP into a sockaddr_in structure.
 *
 * @param host Hostname or IP address.
 * @param port Port number in host byte order.
 * @return sockaddr_in structure for the peer.
 */
struct sockaddr_in get_peer_address(const char *host, uint16_t port);

/**
 * @brief Creates a sockaddr_in structure from IP and port.
 *
 * Port is in network byte order.
 *
 * @param ip IP address.
 * @param port Port number in network byte order.
 * @return sockaddr_in structure.
 */
struct sockaddr_in create_address(struct in_addr ip, uint16_t port);

/**
 * @brief Creates a UDP socket.
 *
 * @return File descriptor of the created socket.
 */
int create_socket();

/**
 * @brief Binds a socket to a given port and optional address.
 *
 * @param socket_fd Socket file descriptor.
 * @param port Port number in host byte order.
 * @param bind_address Optional string address to bind to (NULL for INADDR_ANY).
 */
void bind_socket(int socket_fd, uint16_t port, char* bind_address);

/**
 * @brief Returns current local time in milliseconds using CLOCK_MONOTONIC.
 *
 * @return Time in milliseconds since an unspecified starting point.
 */
uint64_t get_local_time_ms();

/**
 * @brief Returns current time adjusted by start time and offset.
 *
 * @param start_time Start time in milliseconds.
 * @param offset Time offset to subtract.
 * @return Adjusted current time.
 */
uint64_t get_time_now(uint64_t start_time, uint64_t offset);

/**
 * @brief Creates a new Node from address and port.
 *
 * @param peer_address IP address of the peer.
 * @param peer_port Port number of the peer.
 * @return Initialized Node structure.
 */
Node create_node(struct in_addr peer_address, uint16_t peer_port);

/**
 * @brief Finds a peer index in a sorted peer array.
 *
 * @param peer_addr Address of the peer to search for.
 * @param info Pointer to PeerInfo structure.
 * @return Index of the peer if found, otherwise negative insertion point - 1.
 */
int find_peer_index(const struct sockaddr_in *peer_addr, const PeerInfo *info);

/**
 * @brief Adds a new peer or updates an existing peer.
 *
 * @param new_peer Address of the new or updated peer.
 * @param info Pointer to PeerInfo structure.
 */
void add_or_update_peer(const struct sockaddr_in *new_peer, PeerInfo* info);

/**
 * @brief Deletes a peer from the connected peers list.
 *
 * @param peer_index Index of the peer to delete.
 * @param info Pointer to PeerInfo structure.
 */
void delete_peer(int peer_index, PeerInfo* info);

/**
 * @brief Installs a signal handler with given flags.
 *
 * @param signal Signal number to handle.
 * @param handler Pointer to the signal handler function.
 * @param flags Additional flags for sigaction.
 */
void install_signal_handler(int signal, void (*handler)(), int flags);

#endif // COMMON_H
