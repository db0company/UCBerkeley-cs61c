.text

# _start is the entry point into any program.
.global _start
.ent    _start 
_start:


# Very basic memory test of load/store/malloc/free
 
# Allocate 28 bytes in memory, store a string of all lowercase letters in order,
# print the string, free the block, then exit

allocate:
  ori $v0, $0, 61 #set syscall code to 61
  ori $a0, $0, 28 #set num bytes to 28
	syscall
  
set_init_vals:
  addu $t0, $0, $v0 #stores pointer in $t0
  addu $s0, $0, $v0 #stores non moving pointer in $t0
  ori $t1, $0, 97 #sets $t1 to ascii 'a' 
  ori $s1, $0, 123 #sets $s1 to end of lowercase letters for branch
   

store_letter:
  sb $t1, 0($t0) # stores current letter  
  addiu $t1, $t1, 1 #increment letter
  addiu $t0, $t0, 1 #increment pointer 
  bne $t1, $s1, store_letter
  
  ori $t1, $0, 10 # new line char
  sb $t1, 0($t0)
  sb $0, 1($t0) # null terminate the string


print:
	addu $a0, $0, $s0 # move pointer to beginning
	ori   $v0, $0, 4
	syscall  

  
free:
  ori $v0, $0, 62 #set syscall code to free
  addu $a0, $0, $s0 # move block pointer to $a0
  syscall

check_back:
  lb $t0, 27($a0) #should throw an error, bad read
  

# exit the simulation (v0 = 10, syscall)
	ori   $v0, $zero, 10
	syscall

.end _start

