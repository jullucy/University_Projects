# Computational Verification of a Combinatorial Hypothesis

A multi-threaded C application for computationally verifying combinatorial hypotheses related to sumset intersections. This project implements both sequential and parallel approaches to explore mathematical conjectures through exhaustive computational search.

## Table of Contents
- [Overview](#overview)
- [Mathematical Background](#mathematical-background)
- [Architecture](#architecture)
- [Implementation Variants](#implementation-variants)
- [Concurrency Model](#concurrency-model)
- [Memory Management](#memory-management)
- [Build System](#build-system)
- [Usage](#usage)
- [Performance Analysis](#performance-analysis)
- [Algorithm Complexity](#algorithm-complexity)
- [Thread Safety](#thread-safety)
- [Compilation & Execution](#compilation--execution)

## Overview

This project computationally verifies combinatorial hypotheses by systematically exploring sumset combinations and their intersections. The system searches for optimal solutions that satisfy specific mathematical constraints while maximizing certain objective functions.

**Key Features:**
- **Multi-threaded computation** with work-stealing algorithms
- **Memory pool management** for efficient allocation
- **Reference counting** for safe memory deallocation
- **Lock-free data structures** where possible
- **CMake build system** for cross-platform compilation

## Mathematical Background

### Problem Definition
The system explores **sumsets** - mathematical structures representing sums of elements from given sets. The core hypothesis involves:

1. **Sumset Generation**: Creating sumsets A^Σ and B^Σ from base sets
2. **Intersection Analysis**: Computing |A^Σ ∩ B^Σ| for various combinations
3. **Optimization**: Finding configurations that maximize specific metrics
4. **Constraint Satisfaction**: Ensuring trivial/non-trivial intersection conditions

### Mathematical Properties
- **Sumset Definition**: A^Σ = {a₁ + a₂ + ... + aₖ | aᵢ ∈ A}
- **Intersection Constraints**: |A^Σ ∩ B^Σ| = 2 for optimal solutions
- **Sum Equality**: A^Σ.sum = B^Σ.sum for valid candidates
- **Monotonicity**: Sumsets maintain ordered properties for efficient pruning

## Architecture

The system follows a layered architecture with clear separation of concerns:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Input Layer   │────│ Computation     │────│   Output Layer  │
│   (Data Read)   │    │   Engine        │    │  (Solution)     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Common I/O    │    │ Memory Pool     │    │   Thread Pool   │
│   & Sumsets     │    │   Management    │    │   Management    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Core Components
- **Sumset Operations**: Mathematical operations on sumset structures
- **Memory Pool**: Efficient allocation/deallocation of computational nodes  
- **Work Queue**: Thread-safe task distribution system
- **Reference Counting**: Automatic memory management with atomic operations

## Implementation Variants

### 1. Non-Recursive Implementation (`nonrecursive/`)
- **Algorithm**: Iterative depth-first search with explicit stack
- **Memory**: Stack-based frame management
- **Advantages**: Predictable memory usage, no stack overflow risk
- **Use Case**: Single-threaded baseline implementation

#### Key Features:
```c
typedef struct {
    Ref_sumset* a;
    Ref_sumset* b;
} StackFrame;

void solve_iterative(Sumset* start_a, Sumset* start_b, 
                    Solution* best_solution, InputData* input_data, 
                    RefSumsetPool* pool);
```

### 2. Parallel Implementation (`parallel/`)
- **Algorithm**: Multi-threaded work-stealing with lock-free operations
- **Synchronization**: Atomic operations and condition variables
- **Scalability**: Dynamic load balancing across CPU cores
- **Use Case**: High-performance computational verification

#### Key Features:
```c
typedef struct {
    atomic_int ref_count;      // Thread-safe reference counting
    pthread_mutex_t mutex;     // Critical section protection
    pthread_cond_t cond;       // Thread coordination
    int working_counter;       // Active thread tracking
} WorkQueue;
```

## Concurrency Model

### Thread Pool Architecture
- **Worker Threads**: Multiple threads process work items concurrently
- **Work Stealing**: Idle threads steal tasks from busy threads' queues
- **Dynamic Balancing**: Load distribution adapts to computational complexity

### Synchronization Mechanisms

#### Atomic Operations
```c
atomic_int ref_count;                    // Reference counting
atomic_fetch_add(&sumset->ref_count, 1); // Thread-safe increment
atomic_fetch_sub(&sumset->ref_count, 1); // Thread-safe decrement
```

#### Mutex Protection
```c
pthread_mutex_t mutex;        // Critical section protection
pthread_cond_t cond;         // Thread coordination
pthread_cond_wait(&cond, &mutex);  // Wait for work
pthread_cond_broadcast(&cond);     // Notify all threads
```

### Lock-Free Data Structures
- **Memory Pool**: Lock-free allocation using atomic compare-and-swap
- **Reference Counting**: Atomic increment/decrement operations
- **Work Queue**: Minimized lock contention through careful design

## Memory Management

### Reference Counting System
- **Automatic Cleanup**: Objects deallocated when reference count reaches zero
- **Parent-Child Relationships**: Hierarchical sumset dependencies
- **Thread Safety**: Atomic operations ensure consistency

### Memory Pool Implementation
```c
typedef struct {
    Ref_sumset* free_list;      // Available objects
    Ref_sumset* pool_blocks;    // Allocated memory blocks
} RefSumsetPool;

// Pool operations
Ref_sumset* pool_allocate(RefSumsetPool* pool);
void pool_release(RefSumsetPool* pool, Ref_sumset* node);
```

### Benefits
- **Reduced Fragmentation**: Block allocation minimizes heap fragmentation
- **Cache Efficiency**: Locality of reference improves performance
- **Predictable Performance**: Constant-time allocation/deallocation

## Build System

### CMake Configuration
The project uses CMake for cross-platform builds with optimized compilation:

```cmake
# Compiler settings
set(CMAKE_C_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-pthread)

# Release optimizations
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native -O3)
endif()
```

### Build Targets
- **nonrecursive**: Single-threaded iterative implementation
- **parallel**: Multi-threaded concurrent implementation
- **common libraries**: Shared I/O and sumset operations

## Usage

### Basic Execution
```bash
# Build all variants
mkdir build && cd build
cmake ..
make

# Run non-recursive version
./nonrecursive < input.txt

# Run parallel version
./parallel < input.txt
```

### Input Format
```
d n
a₁ a₂ ... aₙ
b₁ b₂ ... bₙ
```
Where:
- `d`: Maximum element value
- `n`: Number of elements in each set
- `aᵢ, bᵢ`: Elements of sets A and B

### Sample Input
```
10 3
1 2 3
4 5 6
```

### Output Format
```
Best solution found:
Sum: 42
Set A^Σ: {elements...}
Set B^Σ: {elements...}
Intersection: {common elements}
```

## Performance Analysis

### Computational Complexity
- **Time Complexity**: O(2^n × d^k) where n is set size, d is max element, k is sumset size
- **Space Complexity**: O(m) where m is maximum active sumsets in memory
- **Parallelization**: Near-linear speedup with proper work distribution

### Optimization Techniques
- **Early Pruning**: Eliminate branches that cannot improve best solution
- **Memory Pooling**: Reduce allocation overhead
- **Cache Optimization**: Maintain data locality for better performance
- **Work Stealing**: Balance computational load across threads

### Benchmarking Results
Performance varies significantly based on input parameters:
- **Small instances** (d ≤ 10): Seconds to minutes
- **Medium instances** (d ≤ 20): Minutes to hours  
- **Large instances** (d > 20): May require distributed computation

## Algorithm Complexity

### Search Space Exploration
The algorithm explores a tree where each node represents a sumset pair (A^Σ, B^Σ):

```
Root: (A₀, B₀)
├── (A₀ + d₁, B₀)
├── (A₀ + d₂, B₀)
│   ├── (A₀ + d₁ + d₂, B₀)
│   └── (A₀ + d₂, B₀ + d₁)
└── ...
```

### Pruning Strategies
1. **Intersection Check**: Skip branches where |A^Σ ∩ B^Σ| ≠ 2
2. **Sum Comparison**: Only explore when A^Σ.sum ≤ B^Σ.sum
3. **Monotonicity**: Exploit ordered properties for early termination
4. **Best Solution Bound**: Prune branches that cannot improve current best

## Thread Safety

### Atomic Operations
All shared data structures use atomic operations for thread safety:
- Reference counting with `atomic_int`
- Memory pool management with compare-and-swap
- Solution updates with proper synchronization

### Critical Sections
Minimal use of mutexes for:
- Work queue management
- Global solution updates
- Thread coordination and termination

### Race Condition Prevention
- **Data Races**: Eliminated through atomic operations
- **Memory Leaks**: Prevented by reference counting
- **Deadlocks**: Avoided through consistent lock ordering

## Compilation & Execution

### Prerequisites
- **GCC** or **Clang** with C17 support
- **CMake** 3.12 or higher
- **pthreads** library
- **Linux/Unix** environment (recommended)

### Build Instructions
```bash
# Clone/navigate to project directory
cd "Computational_Verification_of_a_Combinatorial_Hypothesis"

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build all targets
make

# Optional: Build specific target
make nonrecursive
make parallel
```

### Execution Examples
```bash
# Sequential execution
echo "5 2
1 2
3 4" | ./nonrecursive

# Parallel execution with timing
time echo "8 3
1 2 4
2 3 5" | ./parallel

# Performance comparison
echo "10 4
1 2 3 5
2 4 6 8" | ./nonrecursive > seq_result.txt
echo "10 4
1 2 3 5
2 4 6 8" | ./parallel > par_result.txt
diff seq_result.txt par_result.txt  # Should be identical
```

### Debug Build
```bash
# Debug configuration
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with debugging tools
valgrind --tool=helgrind ./parallel < input.txt
gdb ./parallel
```

## Educational Objectives

This project demonstrates advanced concepts in:

### Concurrent Programming
- **Thread Synchronization**: Mutexes, condition variables, atomic operations
- **Parallel Algorithms**: Work distribution and load balancing
- **Race Condition Handling**: Lock-free programming techniques
- **Deadlock Prevention**: Proper synchronization design

### Systems Programming
- **Memory Management**: Custom allocators and reference counting
- **Performance Optimization**: Cache-aware algorithms and data structures
- **Cross-Platform Development**: CMake and POSIX compliance

### Algorithm Design
- **Search Space Exploration**: Efficient tree traversal strategies
- **Mathematical Computation**: Combinatorial optimization
- **Complexity Analysis**: Time/space trade-offs in parallel computing

---

**Course**: Concurrent Programming  
**Institution**: University of Warsaw (MIMUW)  
**Language**: C  
**Paradigm**: Multi-threaded Computation
