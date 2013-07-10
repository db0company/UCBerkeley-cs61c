	.text
main:
	la	$a0,n1
	la	$a1,n2
	jal	proc2	# proc1 crashes swapb, proc2 makes swapb work
	jal	swapb	# swapa works correctly, swapb throws error
	li	$v0,1	# print n1 and n2; should be 27 and 14
	lw	$a0,n1
	syscall
	li	$v0,11
	li	$a0,' '
	syscall
	li	$v0,1
	lw	$a0,n2
	syscall
	li	$v0,11
	li	$a0,'\n'
	syscall
	li	$v0,10	# exit
	syscall

swapa:
	addiu $sp,    $sp,    -4
	lw $t0, 0($a0)
	sw $t0, 0($sp) # save a0 on stack

	lw $t0, 0($a1) # get value a1
	sw $t0, 0($a0) # save value in a0

	lw $t0, 0($sp) # get value in the stack
	sw $t0, 0($a1) # store value in a1

	addiu $sp, $sp, 4 # restore stack pointer
	jr    $ra

swapb:	
	lw $t0, 0($sp) # get the pointer from the stack

	lw $t1, 0($a0) # get the value in a0
	sw $t1, 0($t0) # put the value at the temporay invalid address that we got from the stack (uninitialized)

	lw $t1, 0($a1)
	sw $t1, 0($a0) # *a0 = *a1

	lw $t1, 0($t0) # get saved value of old a0
	sw $t1, 0($a1) # store in a1

	jr    $ra

proc1:	# custom procedure to crash buggy swap
	addi $t0, $0, 5 # t0 = invalid address
	sw $t0, 0($sp) # put the invalid address on the stack
	jr $ra

proc2:	# custom procedure to guarantee buggy swap works
	addiu $sp,    $sp,    -4 # create something on the stack
	addi $t0, $sp, -4 # address of the storage
	sw $t0, 4($sp) # store the address of this new created storage	

	.data
n1:	.word	14
n2:	.word	27
n3:	.word	0
