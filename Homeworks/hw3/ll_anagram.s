# CS 61C Su13 Homework 3
# Name: Barbara Lepage
# Login: gm
    
    .data
mem:     .space 80 # 4 bytes/word * 20 words
testAB:  .asciiz "Testing linked lists A and B (should be 0): "
testAC:  .asciiz "Testing linked lists A and C (should be 1): "
testAD:  .asciiz "Testing linked lists A and D (should be 0): "
nl:      .asciiz "\n"

# Linked lists for testing:
# A: 1,0,2,2,9,5,NULL
# B: 1,0,2,2,9,1,NULL
# C: 2,9,0,1,5,2,NULL
# D: 1,0,2,2,9,NULL
A5: .word 5 0
A4: .word 9 A5
A3: .word 2 A4
A2: .word 2 A3
A1: .word 0 A2
A0: .word 1 A1

B5: .word 1 0
B4: .word 9 B5
B3: .word 2 B4
B2: .word 2 B3
B1: .word 0 B2
B0: .word 1 B1

C5: .word 2 0
C4: .word 5 C5
C3: .word 1 C4
C2: .word 0 C3
C1: .word 9 C2
C0: .word 2 C1

D4: .word 9 0
D3: .word 2 D4
D2: .word 2 D3
D1: .word 0 D2
D0: .word 1 D1

    .text
    .globl main 

        j main

# Function that sets the values of a block of memory to zero.
# Do NOT modify this function.
# Params:
#  $a0 = beginning of block of memory
#  $a1 = number of words to set to zero
zeroMem:
        beq   $a1,    $zero,  done
        sw    $zero,  0($a0)
        addiu $a0,    $a0,    4
        addi  $a1,    $a1,    -1
        j     zeroMem
done:
        jr $ra

# Function that traverses through a linked list whose values contain
# the numbers 0 to 9 and stores the count of each number into an array.
# Params:
#  $a0 = beginning of the linked list
#  $a1 = a pointer to memory for the array
#    You may assume that the memory block contains enough space for 10 elements

countList:
    # prolog
        # we're not calling functions nor using $s so we don't have to save anything \o/

    # browse the list
        lw    $t0,    0($a0)         # t0 = first item in the list
        lw    $a0,    4($a0)         # a0 = address of the next element

        sll   $t0,    $t0,    2      # t0 = offset = 4 * postion = position << 4
        add   $t0,    $a1,    $t0    # t0 = adress memory at the position t0
        lw    $t1,    0($t0)         # t1 = get what was at this position before
        addi  $t1,    $t1,    1      # t1++
        sw    $t1,    0($t0)         # store the new value at address t0

        bne   $a0,    0,      countList # if next element != 0 goto countList

    # return values
        li    $v0,    0              # store return value

    # epilog
        jr    $ra                    # return

# Function that checks if the numbers (0-9) stored in two linked lists are 
# anagrams of each other (ie. if two lists contains the same number 
# of each int). Each linked list will be at least 1 element long.
# You should first traverse each linked list and store the count of each digit into
# an array (memory for it is given to you), and then check if the two arrays
# are equal.
# Params:
#  $a0 = first element of linked list 1
#  $a1 = first element of linked list 2
#  $a2 = memory segment containing space for 2 arrays of 10 ints each.
#     Note that the memory segment may contain garbage values.
#
# Return (put in $v0): 1 if linked lists are anagrams, 0 if not

isAnagram:

    # prolog
        addiu $sp,    $sp,    -16    # what i will need on the stack
        sw    $ra,    12($sp)        # save pointer to next instruction
        sw    $s0,    8($sp)         # save the $s registers that I will use after
        sw    $s1,    4($sp)
        sw    $s2,    0($sp)

    # keep lists
        move  $s0,    $a0            # s0 = 1st list
        move  $s1,    $a1            # s1 = 2nd list
        move  $s2,    $a2            # s2 = memory array

    # call zeroMem to set memory to 0
        # arguments
        move  $a0,    $s2            # 1st argument = memory
        li    $a1,    20             # 2nd argument = size in words
        # call
        jal   zeroMem

    # call countList for the 1st list
        #arguments
        move  $a0,    $s0            # 1st argument = 1st list
        move  $a1,    $s2            # 2nd argument = memory array
        #call
        jal   countList

    # call countList for the second list
        #arguments
        move  $a0,    $s1            # 1st argument = 2nd list
        addi  $t0,    $a2,    40     # t0 = address of mem + 40 bytes
        move  $a1,    $t0            # 2nd argument = t0
        #call
        jal   countList

    # compare the two array
        # s2 = the address of the 1st array
        # t0 = the address of the 2nd array
        addi  $t0,    $a2,    40     # t0 = address of mem + 40 bytes
        li    $t3,    0              # counter to know when to stop

    isAnagramLoop:
        lw    $t1,    0($s2)         # t1 = element in 1st array
        lw    $t2,    0($t0)         # t2 = element in 2nd array
        bne   $t1,    $t2,    returnNotAnagram # if the elements are different, the lists are not anagrams
        addi  $t3,    $t3,    1      # increment the counter
        beq   $t3,    10,     returnIsAnagram  # if we are at the end of the arrays, we have an anagram
        addi  $s2,    $s2,    4      # increment the pointer of the 1st array
        addi  $t0,    $t0,    4      # increment the pointer of the 2nd array
        j     isAnagramLoop          # loop

    returnNotAnagram:
        li    $v0,    0              # return value = not an anagram
        j     isAnagramEnd

    returnIsAnagram:
        li    $v0,    1              # return value = is an anagram

    isAnagramEnd:
    # epilog
        lw    $s2,    0($sp)         # restore $s registers
        lw    $s1,    4($sp)
        lw    $s0,    8($sp)
        lw    $ra,    12($sp)        # restore pointer to next instruction
        addiu $sp,    $sp,    16     # reset stack
        jr    $ra                    # jump to the next instruction

# Program starts here. Do NOT modify anything below this line.
main:
    la $a0,testAB # Test LL A and B
    li $v0,4
    syscall
    la $a0,A0
    la $a1,B0
    la $a2,mem
    jal isAnagram
    move $a0,$v0
    li $v0,1
    syscall
    la $a0,nl
    li $v0,4
    syscall 
    
    la $a0,testAC # Test LL A and C
    li $v0,4
    syscall
    la $a0,A0
    la $a1,C0
    la $a2,mem
    jal isAnagram
    move $a0,$v0
    li $v0,1
    syscall
    la $a0,nl
    li $v0,4
    syscall

    la $a0,testAD # Test LL A and D
    li $v0,4
    syscall
    la $a0,A0
    la $a1,D0
    la $a2,mem
    jal isAnagram
    move $a0,$v0
    li $v0,1
    syscall

    li $v0 10 # Exit
    syscall
