/**
 * @file messages.h
 * @brief Message types, sizes, and functions for peer-to-peer synchronization protocol.
 */

#ifndef MESSAGES_H
#define MESSAGES_H

/// @name Message types
/// @{
#define MSG_HELLO 1               /**< Hello message. */
#define MSG_HELLO_REPLY 2         /**< Reply to a Hello message. */
#define MSG_CONNECT 3             /**< Connect request message. */
#define MSG_ACK_CONNECT 4         /**< Acknowledgment of Connect message. */
#define MSG_SYNC_START 11         /**< Start synchronization message. */
#define MSG_DELAY_REQUEST 12      /**< Delay request message. */
#define MSG_DELAY_RESPONSE 13     /**< Delay response message. */
#define MSG_LEADER 21             /**< Leader announcement message. */
#define MSG_GET_TIME 31           /**< Request for current time. */
#define MSG_TIME 32               /**< Response with current time. */
/// @}

/// @name Message sizes (in bytes)
/// @{
#define HELLO_SIZE 1
#define MIN_HELLO_REPLY_SIZE 3
#define CONNECT_SIZE 1
#define ACK_CONNECT_SIZE 1
#define SYNC_START_SIZE 10
#define DELAY_REQUEST_SIZE 1
#define DELAY_RESPONSE_SIZE 10
#define LEADER_SIZE 2
#define GET_TIME_SIZE 1
#define TIME_SIZE 10
/// @}

/// @name Basic field sizes
/// @{
#define MSG_SIZE 1
#define COUNT_SIZE 2
#define TIMESTAMP_SIZE 8
#define PORT_SIZE 2
#define IPV4_SIZE 4
/// @}

/// @name Synchronization levels
/// @{
#define SYNC_LEADER 0             /**< Node is a synchronization leader. */
#define SYNC_NOT_SYNCED 255       /**< Node is not synchronized. */
#define MAX_SYNCED 254            /**< Maximum sync level before becoming a leader. */
/// @}

/// @name Constants
/// @{
#define BUF_SIZE 65507            /**< Maximum UDP payload size. */
#define FLAGS 0                   /**< Default flags for send/recv functions. */
#define SYNC_INTERVAL 5000        /**< Synchronization interval in milliseconds. */
#define LEADER_SHORT_TIMEOUT 3000 /**< Time to subtract from time to send sync_start after 2s. */
#define MAX_PORT 65535            /**< Maximum valid port number. */
#define MIN_PORT 1                /**< Minimum valid port number. */
/// @}

/// @name Validation return values
/// @{
#define SUCCESS 1
#define FAILURE 0
/// @}

/** @brief Sends a HELLO message to a peer.
 *
 * This message contains:
 * - Message type (1 byte).
 *
 * @param socket_fd File descriptor of the socket.
 * @param peer_addr Address of the peer to send to.
 */
void send_hello(int socket_fd, const struct sockaddr_in *peer_addr);

/** @brief Sends a HELLO_REPLY message to a peer.
 *
 * This message contains:
 * - Message type (1 byte).
 * - The number of connected peers (2 bytes).
 * - Peer records, every record contains: address length (1 byte),
 *   address (4 bytes), and port (2 bytes).
 *
 * @param socket_fd File descriptor of the socket.
 * @param peer_addr Address of the peer to send to.
 * @param info Pointer to local peer information.
 * @param buffer Buffer to hold the outgoing message.
 */
void send_hello_reply(int socket_fd, const struct sockaddr_in *peer_addr,
    PeerInfo* info, uint8_t* buffer);

/** @brief Handles a received HELLO_REPLY message.
 *
 * It reads the list of connected peers and if the whole message is valid,
 * sends CONNECT messages to them.
 * This message contains:
 * - Message type (1 byte).
 *
 * @param buffer Received data buffer.
 * @param len Length of the received buffer.
 * @param sender Address of the sender.
 * @param socket_fd File descriptor of the socket.
 * @param peer_info Pointer to peer list.
 * @param waiting_to_connect Pointer to peers waiting to connect.
 */
void receive_hello_reply(const uint8_t *buffer, size_t len,
    const struct sockaddr_in *sender, int socket_fd, PeerInfo* peer_info,
    PeerInfo* waiting_to_connect);

/** @brief Handles a received CONNECT message.
 *
 * It adds the sender to the peer list and sends an ACK_CONNECT message back.
 * This message contains:
 * - Message type (1 byte).
 *
 * @param sender Address of the sender.
 * @param info Pointer to local peer information.
 * @param socket_fd File descriptor of the socket.
 */
void receive_connect(struct sockaddr_in *sender, PeerInfo *info, int socket_fd);

/** @brief Handles a received ACK_CONNECT message.
 *
 * It adds the sender to the peer list.
 *
 * @param sender Address of the sender.
 * @param info Pointer to local peer information.
 */
void receive_ack_connect(const struct sockaddr_in *sender, PeerInfo *info);

/** @brief Sends a SYNC_START message to synchronize clocks.
 *
 * This message contains:
 * - Message type (1 byte).
 * - Synchronization level (1 byte).
 * - Timestamp (8 bytes).
 *
 * @param my_info Pointer to local peer information.
 * @param peer_info Pointer to peer list.
 * @param synch_info Pointer to synchronization information.
 */
void send_sync_start(MyInfo *my_info, PeerInfo *peer_info, SynchInfo* synch_info);

/** @brief Handles a received SYNC_START message.
 *
 * Validate a SYNC_START message from peers. Check if we should synchronize with
 * them. If so, send a DELAY_REQUEST message back.
 * This message contains:
 * - Message type (1 byte).
 *
 * @param buffer Received data buffer.
 * @param sender Address of the sender.
 * @param my_info Pointer to local node information.
 * @param synch_info Pointer to synchronization information.
 * @param peer_info Pointer to peer list.
 */
void receive_sync_start(const uint8_t *buffer, const struct sockaddr_in *sender,
    MyInfo *my_info, SynchInfo* synch_info, PeerInfo* peer_info);

/** @brief Sends a DELAY_RESPONSE message to a peer.
 *
 * This message contains:
 * - Message type (1 byte).
 * - Synchronization level (1 byte).
 * - Timestamp (8 bytes).
 *
 * @param receiver Address of the peer to send to.
 * @param info Pointer to local node information.
 */
void send_delay_response(struct sockaddr_in *receiver, const MyInfo* info);

/** @brief Handles a received DELAY_RESPONSE message.
 *
 * If the message is valid, calculates the new time offset and
 * updates the synchronization level.
 *
 * @param buffer Received data buffer.
 * @param info Pointer to local node information.
 * @param synch_info Pointer to synchronization information.
 * @param peer_info Pointer to peer list.
 */
void receive_delay_response(const uint8_t *buffer, MyInfo* info,
    SynchInfo* synch_info, PeerInfo* peer_info);

/** @brief Handles a received LEADER message.
 *
 * If the message is valid, updates the synchronization level.
 * Leader message contains:
 * - Message type - LEADER (1 byte),
 * - Synchronization level (1 byte).
 * Synchronization level can be either:
 *  - 0 - a signal to become a leader,
 *  - 255 - a signal to stop being a leader (valid only if is sent to leader).
 *
 * @param buffer Received data buffer.
 * @param len Length of the received buffer.
 * @param info Pointer to local node information.
 * @param synch_info Pointer to synchronization information.
 */
void receive_leader(const uint8_t *buffer, ssize_t len, MyInfo* info,
    SynchInfo* synch_info);

/** @brief Sends a TIME response message to a peer.
 *
 * @param receiver Address of the peer to send to.
 * @param info Pointer to local node information.
 */
void send_time_response(struct sockaddr_in *receiver, const MyInfo* info);

#endif //MESSAGES_H