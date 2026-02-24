.data
    fmt_bar:
    .string "|%s|\n"
print:
    .string "%d\n"

msg_no_space:
    .string "error: invalid input (no space)\n"
buf: 
    .space 512

.section .text
.globl main

main:
    la a0, buf
    li a1, 512
    call gets

    la t0, buf
    
remove_nl:
    lbu t1, 0(t0)
    beqz t1, find_space
    li t2, 10       
    beq t1, t2, put0
    addi t0, t0, 1
    j remove_nl

put0:
    sb zero, 0(t0)

find_space:
    la a0, buf
    li a1, ' '
    call strchr
    beqz a0, no_space

    mv t5, a0

    la a0, fmt_bar
    la a1, buf
    call printf
    
    sb zero, 0(t5)   
    addi t5, t5, 1    

    la a0, buf
    call atoi
    mv t3, a0

    mv a0, t5
    call atoi
    mv t0, a0

    add t1, t3, t0

    la a0, print
    mv a1, t1
    call printf

    li a0, 0
    call exit

no_space:
    li a0, 2
    la a1, msg_no_space
    call fprintf
    li a0, 1
    call exit