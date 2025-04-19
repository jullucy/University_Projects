global mdiv

; Arguments of mdiv function:
;   rdi - pointer to x
;   rsi - value of n
;   rdx - value of y

mdiv:
    ; Prepare registers needed for the division.
    xor r8, r8                           ; In r8 we will store the sign of the dividend, 0 if it is
                                         ; positive, or to 1 if it is negative.
    xor r9, r9                           ; In r9 we will store the sign of the divider, 0 if it is
                                         ; positive, 1 if it is negative.
    mov r11, rsi                         ; Used to hold the number of elements in the array.
    mov r10, rdx                         ; Move the divider to r10, because rdx will be needed in
                                         ; further divisions.
    mov rax, qword [rdi + rsi*8 - 8]     ; Takes the last element of the array.
    test rax, rax                        ; Checks if the highest bit is set, if the number is negative.
    jns .check_divider                   ; If dividend is positive, so the highest bit is not set
                                         ; we don't have to change anything (r8 is set to 0 already).

    ; If the dividend was less than zero we change r8 to 1.
    mov r8, 1

    ; In order to change the number to natural binary code we have to negate all
    ; the bits and add one.
    ; The number is written in little-endian order, so we have to start adding
    ; one from the beginning of the array (and move one if there is overflow).
.change_sign:
    not qword [rdi + rsi*8 - 8]          ; Negate next element.
    dec rsi
    jnz .change_sign                     ; Continue until all elements are proceed.

    ; When rsi is zero, all bits were negated, so we have to add one to the
    ; number written in the array.
    mov rcx, 0xffffffffffffffff          ; Move -1 ro rcx, which will be a loop counter.
.add_loop:
    inc rcx
    cmp rcx, r11                         ; Check if all elements were proceed - that means there is a
    ; possibility of overflow.
    je .check_overflow
    add qword [rdi + rcx*8], 1           ; Add one to the currently proceeded element.
    jz .add_loop                         ; If there was an overflow, continue the loop.

    ; If there was no overflow while adding, we have to check the possibility
    ; of overflow, while dividing the biggest number by -1.
    inc rcx
    cmp rcx, r11                         ; Check if that was the last element.
    jne .check_divider ; If not all elements before the last one were zero
    ; there is no risk of overflow.
.check_overflow:
    mov rax, qword [rdi + rcx*8 - 8]     ; Put the last element of the array in rax.
    test rax, rax                        ; Check if the highest bit of this element is set - that
                                         ; means there is a possibility of overflow.
    jns .check_divider                   ; If it is not set there is no risk of overflow.
    add rdx, 1                           ; Add one to the divider in order to check if it is -1.
    jnz .check_divider                   ; If the divider is equal to 0 after adding one,
                                         ; there is an overflow.
    div rsi                              ; Divide by 0 to cause the segfault error.
.check_divider:
    mov rsi, r11                         ; Load the number of elements to rsi.
    xor rdx, rdx                         ; rdx must be equal to zero at the beginning of the division.
    test r10, r10                        ; Checks if the divider is negative, the same way as before.
    jns .divide                          ; If dividend is positive, we don't have to change anything,
                                         ; because r9 is already set to 0.

    ; If divider is negative, change number to natural binary code:
    neg r10
    mov r9, 1                            ; The divider was less than zero, so we changed r9 to 1.
.divide:
    mov rax, qword [rdi + rsi*8 - 8]     ; Load the next element to rax.
    dec rsi
    div r10                              ; Performs the division, with lower chunk in rax and potentially
                                         ; remainer from previous division in rdx.

    mov qword [rdi + rsi*8], rax         ; Save quotient in the array.
    cmp r9, r8                           ; Checks sign of the quotient.
    je .save_values

    ; If quotient is negative:
    not qword [rdi + rsi*8]              ; Change back to u2, to make the quotient negative.
    test rsi, rsi                        ; Check if it is the last number in the array.
    jne .save_values

    mov rcx, 0xffffffffffffffff          ; Move -1 ro rcx, which will be a loop counter.
.loop:
    ; Loop that adds one to the number written in array (loop is needed,
    ; because we have to move the carry from lower bits).
    inc rcx
    cmp rcx, r11                         ; Check if all the elements of the array were to proceed.
    je .save_values                      ; If all elements were to proceed end the loop.
    add qword [rdi + rcx*8], 1           ; Add one to the next element.
    jz .loop                             ; If there were an overflow, continue the loop.
.save_values:
    mov rax, rdx                         ; Move the remainder to rax.
    test r8, r8                          ; Check if the remainder should be negative.
    je .end                              ; If it is positive we don't have to do anything.

    ; If remainder is negative:
    neg rax                              ; Change back to u2, to make the remainder negative.
.end:
    test rsi, rsi                        ; Check if that was the last element of the array.
    jnz .divide                          ; Jump to next division if it wasn't.

    ret                                  ; End the program if it was.
