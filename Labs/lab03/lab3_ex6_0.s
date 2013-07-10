	.file	1 "lab3_ex6.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.gnu_attribute 4, 1
	.globl	source
	.data
	.align	2
	.type	source, @object
	.size	source, 28
source:
	.word	3
	.word	1
	.word	4
	.word	1
	.word	5
	.word	9
	.word	0

	.comm	dest,40,4
	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$fp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-16
	sw	$fp,12($sp)
	move	$fp,$sp
	sw	$0,0($fp)
	j	$L2
$L3:
	lw	$4,0($fp)
	lw	$3,0($fp)
	lui	$2,%hi(source)
	sll	$3,$3,2
	addiu	$2,$2,%lo(source)
	addu	$2,$3,$2
	lw	$3,0($2)
	lui	$2,%hi(dest)
	sll	$4,$4,2
	addiu	$2,$2,%lo(dest)
	addu	$2,$4,$2
	sw	$3,0($2)
	lw	$2,0($fp)
	addiu	$2,$2,1
	sw	$2,0($fp)
$L2:
	lw	$3,0($fp)
	lui	$2,%hi(source)
	sll	$3,$3,2
	addiu	$2,$2,%lo(source)
	addu	$2,$3,$2
	lw	$2,0($2)
	bne	$2,$0,$L3
	move	$2,$0
	move	$sp,$fp
	lw	$fp,12($sp)
	addiu	$sp,$sp,16
	j	$31
	.end	main
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.1"
