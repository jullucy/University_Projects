# NAND Gates Simulation

A dynamic library implementation for simulating combinational Boolean circuits composed entirely of NAND gates. This project demonstrates low-level memory management and digital circuit simulation in C.

## Overview

This library provides a complete toolkit for creating, connecting, and analyzing Boolean circuits using only NAND gates - a functionally complete gate type that can implement any Boolean function.

**Key Features:**
- **NAND Gate Creation**: Dynamic allocation of gates with configurable input count
- **Circuit Connection**: Connect gate outputs to other gate inputs or Boolean signals
- **Critical Path Analysis**: Calculate the longest propagation delay through the circuit
- **Memory Management**: Efficient pooling and reference counting for large circuits
- **Error Handling**: Comprehensive error checking with errno-based reporting

## Library Functions

### Gate Management
- `nand_new(n)` - Creates a new NAND gate with n inputs
- `nand_delete(gate)` - Safely removes gate and all its connections

### Circuit Connection  
- `nand_connect_nand(out_gate, in_gate, input_num)` - Connect gate output to gate input
- `nand_connect_signal(signal, gate, input_num)` - Connect Boolean signal to gate input

### Circuit Analysis
- `nand_evaluate(gates[], signals[], count)` - Evaluate circuit and find critical path
- `nand_fan_out(gate)` - Count number of gates connected to this gate's output
- `nand_input(gate, input_num)` - Get what's connected to specified input
- `nand_output(gate, index)` - Iterate through gates connected to output

## Technical Implementation

### Memory Management
- **Reference Counting**: Automatic cleanup when gates are no longer referenced
- **Connection Tracking**: Bidirectional links between connected gates
- **Dynamic Arrays**: Resizable arrays for gate connections
- **Error Recovery**: Proper cleanup on memory allocation failures

### Circuit Evaluation
- **Critical Path**: Calculates maximum delay from inputs to outputs
- **Cycle Detection**: Prevents infinite loops in circuit evaluation  
- **Signal Propagation**: Simulates Boolean logic through NAND operations
- **Visited Tracking**: Optimizes evaluation by avoiding redundant calculations

## Build Instructions

```bash
# Compile the shared library
make

# This creates:
# - libnand.so: The main library
# - nand_example: Example program demonstrating usage
```

## Usage Example

```c
#include "nand.h"

// Create gates
nand_t* gate1 = nand_new(2);  // 2-input NAND gate  
nand_t* gate2 = nand_new(2);  // 2-input NAND gate

// Connect signals
bool input_a = true;
bool input_b = false;
nand_connect_signal(&input_a, gate1, 0);
nand_connect_signal(&input_b, gate1, 1);

// Connect gates
nand_connect_nand(gate1, gate2, 0);
nand_connect_signal(&input_a, gate2, 1);

// Evaluate circuit
nand_t* gates[] = {gate2};
bool outputs[1];
ssize_t critical_path = nand_evaluate(gates, outputs, 1);

printf("Critical path length: %zd\n", critical_path);
printf("Output: %s\n", outputs[0] ? "true" : "false");

// Cleanup
nand_delete(gate1);
nand_delete(gate2);
```

## Error Handling

The library uses errno to report errors:
- `ENOMEM`: Memory allocation failure
- `EINVAL`: Invalid parameters (NULL pointers, out-of-range indices)
- `ECANCELED`: Circuit evaluation failed (cycles, unconnected inputs)

## Applications

This library can be used to:
- **Digital Circuit Design**: Prototype and analyze logic circuits
- **Computer Architecture**: Understand gate-level implementations
- **Algorithm Testing**: Verify Boolean function implementations
- **Educational**: Learn about digital logic and memory management

---

**Course**: Computer Architecture and Operating Systems  
**Institution**: University of Warsaw (MIMUW)  
**Language**: C17  
**Build System**: GNU Make

*This project demonstrates systems programming concepts including dynamic libraries, memory management, and digital circuit simulation.*
