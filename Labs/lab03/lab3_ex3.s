        .data
        .word 2, 4, 6, 8
n:      .word 9


        .text
main: 	add     $t0, $0, $zero # t0 = 0
	addi    $t1, $zero, 1 # t1 = 1
	la      $t3, n # load address t3 = &n (n = 9)
	lw      $t3, 0($t3) # t3 = load t3 (t3 = 9)
fib: 	beq     $t3, $0, finish # if t3 = 0 goto finish
	add     $t2,$t1,$t0 # t2 = t1 + t0
	move    $t0, $t1 # t0 = t1
	move    $t1, $t2 # t1 = t2
	subi    $t3, $t3, 1 # t3--
	j       fib # goto fib (loop)
finish: addi    $a0, $t0, 0 # a0 = t0
	li      $v0, 1 # load immidiate v0 = 1
	syscall # ???
	li      $v0, 10	# load immidiate v0 = 10
	syscall # ???

