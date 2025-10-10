# P2P Clock Synchronization

A distributed peer-to-peer clock synchronization system implemented in C using UDP sockets. This project demonstrates network programming concepts, distributed algorithms, and time synchronization protocols for maintaining coherent time across multiple networked nodes.

## Table of Contents
- [Overview](#overview)
- [Protocol Design](#protocol-design)
- [Message Types](#message-types)
- [Network Architecture](#network-architecture)
- [Synchronization Algorithm](#synchronization-algorithm)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Command Line Options](#command-line-options)
- [Network Discovery](#network-discovery)
- [Time Synchronization Process](#time-synchronization-process)
- [Error Handling](#error-handling)
- [Technical Implementation](#technical-implementation)

## Overview

This system implements a decentralized clock synchronization protocol where nodes in a peer-to-peer network coordinate to maintain synchronized clocks. Each node can join the network, discover peers, and participate in time synchronization without requiring a central time server.

**Key Features:**
- **Peer Discovery**: Automatic discovery and connection to network peers
- **Distributed Synchronization**: No single point of failure or central coordinator
- **UDP Communication**: Lightweight, connectionless network protocol
- **Leader Election**: Dynamic selection of time reference nodes
- **Fault Tolerance**: Graceful handling of node failures and network partitions
- **Time Offset Calculation**: Precise time difference computation between nodes

## Protocol Design

### Network Topology
The system operates as an overlay network where nodes maintain connections to multiple peers, forming a mesh topology that provides redundancy and fault tolerance.

### Synchronization Levels
Nodes maintain synchronization levels indicating their time accuracy:
- **Level 0**: Not synchronized (initial state)
- **Level 1-254**: Synchronized with increasing accuracy levels
- **Level 255**: Leader node (time reference)

## Message Types

### Connection Management
- **HELLO** (`MSG_HELLO`): Initial contact message when joining network
- **HELLO_REPLY** (`MSG_HELLO_REPLY`): Response containing list of known peers
- **CONNECT** (`MSG_CONNECT`): Request to establish peer connection
- **ACK_CONNECT** (`MSG_ACK_CONNECT`): Acknowledgment of successful connection

### Time Synchronization
- **SYNC_START** (`MSG_SYNC_START`): Initiate synchronization with timestamp
- **DELAY_REQUEST** (`MSG_DELAY_REQUEST`): Request for delay measurement
- **DELAY_RESPONSE** (`MSG_DELAY_RESPONSE`): Response with timestamp for delay calculation

### Network Coordination
- **LEADER** (`MSG_LEADER`): Announce or transfer leadership
- **GET_TIME** (`MSG_GET_TIME`): Request current time from peer
- **TIME** (`MSG_TIME`): Response with current synchronized time

## Network Architecture

```
    Node A (Leader)          Node B (Level 2)
         │                        │
         ├─── SYNC_START ────────▶│
         │                        │
         │◄─── DELAY_REQUEST ─────┤
         │                        │
         ├─── DELAY_RESPONSE ────▶│
         │                        │
    Node C (Level 1)         Node D (Level 3)
         │                        │
         └─── HELLO_REPLY ───────▶│
```

## Synchronization Algorithm

### Time Offset Calculation
The system uses a two-way message exchange to calculate network delay and time offset:

1. **Sync Request**: Node A sends `SYNC_START` with timestamp T1
2. **Delay Measurement**: Node B requests delay measurement at time T2
3. **Response**: Node A responds with timestamp T3
4. **Offset Calculation**: 
   ```
   network_delay = (T3 - T1) / 2
   time_offset = T2 - T1 - network_delay
   ```

### Synchronization Process
1. **Peer Selection**: Choose best synchronized peer (highest sync level)
2. **Time Exchange**: Perform two-way timestamp exchange
3. **Offset Application**: Adjust local clock with calculated offset
4. **Level Update**: Update synchronization level based on peer's level

## Build Instructions

### Prerequisites
- **GCC** compiler with C support
- **GNU Make**
- **POSIX-compliant** system (Linux, macOS, Unix)

### Compilation
```bash
# Navigate to project directory
cd P2P_clock_synchronization

# Build the project
make

# This creates the executable: peer-time-sync
```

### Clean Build
```bash
# Remove compiled files
make clean

# Rebuild from scratch
make clean && make
```

## Usage

### Basic Execution
```bash
# Start first node (becomes leader)
./peer-time-sync -p 8000

# Join existing network
./peer-time-sync -p 8001 -a localhost -r 8000

# Bind to specific interface
./peer-time-sync -b 192.168.1.100 -p 8002 -a 192.168.1.10 -r 8000
```

### Network Formation Example
```bash
# Terminal 1: Start bootstrap node
./peer-time-sync -p 5000

# Terminal 2: Join as second node
./peer-time-sync -p 5001 -a localhost -r 5000

# Terminal 3: Join through any existing node
./peer-time-sync -p 5002 -a localhost -r 5001

# Terminal 4: Create another entry point
./peer-time-sync -p 5003 -a localhost -r 5000
```

## Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `-b <address>` | Bind to specific local address | `-b 192.168.1.10` |
| `-p <port>` | Local port number to bind | `-p 8000` |
| `-a <address>` | Peer address to connect to | `-a example.com` |
| `-r <port>` | Peer port to connect to | `-r 8000` |

### Parameter Rules
- **Bootstrap Node**: Use only `-p` (and optionally `-b`)
- **Joining Node**: Must specify both `-a` and `-r` together
- **Port Range**: 1-65535 for peer ports, 0-65535 for local ports
- **Address Format**: IPv4 addresses, hostnames, or `localhost`

## Network Discovery

### Bootstrap Process
1. **Initial Contact**: New node sends `HELLO` to known peer
2. **Peer List**: Receives `HELLO_REPLY` with list of network nodes
3. **Connection Requests**: Sends `CONNECT` to multiple peers
4. **Acknowledgments**: Receives `ACK_CONNECT` to confirm connections
5. **Network Integration**: Begins participating in synchronization

### Peer Management
- **Dynamic Connections**: Nodes maintain connections to multiple peers
- **Connection Limits**: Maximum of 65,535 concurrent peer connections
- **Failure Detection**: Timeout-based detection of unresponsive peers
- **Network Resilience**: Automatic reconnection and peer discovery

## Time Synchronization Process

### Synchronization Phases

#### 1. Sync Initiation
```c
// Send SYNC_START every 5 seconds if not at max sync level
if (synchronization_level < 254 && 
    time_since_last_sync > SYNC_START_INTERVAL) {
    send_sync_start_to_best_peer();
}
```

#### 2. Delay Measurement
```c
// Measure network delay using two-way message exchange
T1 = send_sync_start_time;
T2 = receive_delay_request_time;  
T3 = send_delay_response_time;
network_delay = (T3 - T1) / 2;
```

#### 3. Time Adjustment
```c
// Calculate and apply time offset
time_offset = peer_time - local_time - network_delay;
synchronized_time = local_time + time_offset;
```

### Leader Election
- **Automatic Selection**: Node with highest sync level becomes leader
- **Leader Responsibilities**: Provides time reference for other nodes
- **Leader Transitions**: Graceful handoff when better time source appears

## Error Handling

### Network Errors
- **Socket Failures**: Automatic socket recreation and rebinding
- **Message Corruption**: Checksum validation and message replay
- **Peer Unreachable**: Timeout detection and peer removal
- **Network Partitions**: Graceful handling of network splits

### Synchronization Errors
- **Clock Drift**: Periodic resynchronization to correct drift
- **Invalid Timestamps**: Rejection of obviously incorrect time values
- **Circular References**: Prevention of synchronization loops
- **Timeout Handling**: Fallback to unsynchronized state after timeout

### Signal Handling
```c
// Graceful shutdown on SIGINT (Ctrl+C)
signal(SIGINT, catch_int);

// Clean resource deallocation
void cleanup() {
    close(socket_fd);
    free(peer_list);
    free(message_buffers);
}
```

## Technical Implementation

### Data Structures

#### Node Information
```c
typedef struct {
    uint64_t clock_start;           // Node startup time
    uint8_t synchronization_level;  // Current sync level (0-255)
    int64_t time_offset;           // Time adjustment offset
    int socket_fd;                 // UDP socket descriptor
} MyInfo;
```

#### Peer Management
```c
typedef struct {
    Node* connected_nodes;          // Dynamic peer array
    size_t connected_nodes_size;    // Array capacity
    uint16_t peer_count;           // Active peer count
    int synchronized_peer;          // Primary sync peer index
} PeerInfo;
```

### Network Programming
- **UDP Sockets**: Non-blocking, connectionless communication
- **Poll-based I/O**: Efficient handling of multiple message sources
- **Binary Protocol**: Compact message format for network efficiency
- **Endianness Handling**: Consistent byte order across different architectures

### Time Management
```c
// High-precision timestamp generation
uint64_t get_local_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// Synchronized time calculation
uint64_t get_synchronized_time(MyInfo* info) {
    return get_local_time_ms() + info->time_offset;
}
```

### Memory Management
- **Dynamic Arrays**: Resizable peer lists for scalability
- **Buffer Pooling**: Reusable message buffers for efficiency
- **Resource Cleanup**: Proper deallocation on shutdown
- **Error Recovery**: Memory cleanup on allocation failures

## Performance Characteristics

### Scalability
- **Peer Limit**: Up to 65,535 concurrent connections per node
- **Message Overhead**: Minimal protocol overhead (1-10 bytes per message)
- **Memory Usage**: O(n) where n is number of connected peers
- **CPU Usage**: Low overhead with poll-based I/O

### Timing Accuracy
- **Synchronization Precision**: Millisecond-level accuracy
- **Network Delay Compensation**: Two-way delay measurement
- **Clock Drift Correction**: Periodic resynchronization
- **Convergence Time**: Network-wide sync typically within seconds

---

**Course**: Computer Networks  
**Institution**: University of Warsaw (MIMUW)  
**Language**: C17  
**Protocol**: UDP-based P2P synchronization

*This project demonstrates distributed systems concepts including peer-to-peer networking, consensus algorithms, and network time synchronization protocols.*
