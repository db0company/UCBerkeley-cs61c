.text

# _start is the entry point into any program.
.global _start
.ent    _start 
_start:

#
#  The header ends here, and code goes below
#

# First Fit Malloc Testing

set_sizes:
  ori $s0, $0, 1024
  ori $s1, $0, 512
  ori $s2, $0, 256
  ori $s3, $0, 2048
  ori $s4, $0, 1
  ori $s5, $0, 16384 #entire size of heap

  
body:
  add $a0, $s0, $0
  jal allocate
  beq $0, $v0, alloc_failed
  
  add $s6, $v0, $0 #$s6 points to beginning of heap
  add $a0, $s1, $0
  jal allocate
  beq $0, $v0, alloc_failed
  
  add $a0, $s6, $0 #move first block pointer to a0
  jal free
  
  add $a0, $s2, $0
  jal allocate
  beq $0, $v0, alloc_failed
  
  add $a0, $s3, $0
  jal allocate
  beq $0, $v0, alloc_failed
  
  add $a0, $s4, $0
  jal allocate
  beq $0, $v0, alloc_failed


  add $a0, $s5, $0
  jal allocate
  beq $0, $v0, alloc_failed  # this one should fail
  
  j exit # SHOULD NOT REACH THIS LINE



allocate:
  ori $v0, $0, 61 #set syscall code to ff_malloc
	syscall
  jr $ra


free:
  ori $v0, $0, 62 #set syscall code to free
	syscall
  jr $ra   

alloc_failed:
  or $a0, $0, $0 #zero out
  ori $a0, %lo(str)
  ori $v0, $0, 4
  syscall # print the string

# exit the simulation (v0 = 10, syscall)
exit:
	ori   $v0, $zero, 10
	syscall

.end _start

.data
str: .asciiz "ALLOCATION FAILED!\n"
