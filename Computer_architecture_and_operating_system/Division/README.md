# Multi-Precision Integer Division (mdiv)

An x86-64 assembly implementation of division for arbitrarily large integers stored as arrays. This project demonstrates low-level arithmetic operations and two's complement number handling in assembly language.

## Overview

The `mdiv` function performs division of large integers that exceed the capacity of standard CPU registers. Numbers are represented as arrays of 64-bit words in little-endian format, allowing for arithmetic on integers of arbitrary size.

**Key Features:**
- **Multi-precision arithmetic**: Handle integers larger than 64 bits
- **Two's complement support**: Proper handling of negative numbers
- **Overflow detection**: Prevents division overflow errors
- **Assembly optimization**: Direct x86-64 assembly for maximum performance

## Function Signature

```assembly
global mdiv

; Arguments:
;   rdi - pointer to dividend array (modified in-place with quotient)
;   rsi - number of 64-bit elements in the array
;   rdx - divisor (64-bit signed integer)
;
; Returns:
;   rax - remainder of the division
```

## Algorithm Description

### Input Processing
1. **Sign Detection**: Determines signs of dividend and divisor
2. **Two's Complement Conversion**: Converts negative numbers to positive
3. **Overflow Check**: Detects potential overflow (dividing minimum value by -1)

### Division Process
1. **Long Division**: Processes array elements from most to least significant
2. **Remainder Propagation**: Carries remainder between division steps
3. **Quotient Storage**: Stores results back in the original array

### Output Formatting
1. **Sign Application**: Applies correct sign to quotient based on operand signs
2. **Two's Complement**: Converts negative results back to two's complement
3. **Remainder Sign**: Ensures remainder has same sign as dividend

## Technical Details

### Number Representation
- **Little-endian arrays**: Least significant word at index 0
- **64-bit elements**: Each array element is a `qword` (8 bytes)
- **Two's complement**: Negative numbers use standard two's complement format

### Assembly Techniques
- **Register Management**: Efficient use of x86-64 registers
- **Conditional Jumps**: Branch prediction-friendly control flow
- **Arithmetic Instructions**: Direct use of `div`, `neg`, `add` instructions
- **Memory Operations**: Direct array manipulation without function calls

### Edge Cases Handled
- **Division by zero**: Triggers segmentation fault as expected
- **Overflow detection**: Prevents undefined behavior on overflow
- **Negative operands**: Proper two's complement arithmetic
- **Large numbers**: Handles arrays of any reasonable size

## Usage Example

```c
#include <stdint.h>

// Assembly function declaration
extern int64_t mdiv(int64_t* dividend, size_t n, int64_t divisor);

int main() {
    // Example: divide 0x1FFFFFFFFFFFFFFFF (129-bit number) by 3
    int64_t number[3] = {
        0xFFFFFFFFFFFFFFFF,   // Lower 64 bits
        0xFFFFFFFFFFFFFFFF,   // Middle 64 bits  
        0x1FFFFFFFFFFFFFFF    // Upper 65 bits (with sign bit)
    };
    
    int64_t remainder = mdiv(number, 3, 3);
    
    // number[] now contains the quotient
    // remainder contains the division remainder
    
    return 0;
}
```

## Build Instructions

```bash
# Assemble the code
nasm -f elf64 mdiv.asm -o mdiv.o

# Link with C code (example)
gcc -o test_program main.c mdiv.o
```

## Mathematical Properties

### Division Algorithm
The implementation follows the standard long division algorithm:
```
For each 64-bit chunk from most to least significant:
    remainder = (remainder << 64) + current_chunk
    quotient_chunk = remainder / divisor  
    remainder = remainder % divisor
```

### Sign Rules
- **Quotient sign**: Negative if operands have different signs
- **Remainder sign**: Always matches dividend sign
- **Overflow**: Occurs only when dividing minimum value by -1

## Performance Characteristics

- **Time Complexity**: O(n) where n is the number of array elements
- **Space Complexity**: O(1) - division performed in-place
- **Cache Efficiency**: Sequential memory access pattern
- **Branch Prediction**: Minimal conditional branching

## Applications

This function is useful for:
- **Cryptographic calculations**: RSA, elliptic curve operations
- **High-precision mathematics**: Scientific computing applications  
- **Financial systems**: Precise decimal arithmetic
- **Big integer libraries**: Foundation for arbitrary-precision math

---

**Course**: Computer Architecture and Operating Systems  
**Institution**: University of Warsaw (MIMUW)  
**Language**: x86-64 Assembly (NASM syntax)  
**Architecture**: Intel/AMD 64-bit processors

*This project demonstrates low-level arithmetic implementation and understanding of computer number representation systems.*
