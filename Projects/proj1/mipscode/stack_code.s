.text

# _start is the entry point into any program.
.global _start
.ent    _start 
_start:


#Read various memory locations in the code and stack section.
#Then write to the stack (OK), then write to the code section (BAD).

            
  ori $sp, $0, 32768 # sets $sp to 0x8000
  addiu $sp, $sp, 4096 # sets $sp to 0x9000
  
  lui $t0, 57005 # sets to 0xdead0000
  lui $t1, 48879 # sets %t1=0xbeef0000
  srl $t1, $t1, 16 # sets $t1=0x0000beef
  add $t2, $t1, $t0 # sets $t2=0xdeadbeef
  
  sw $t2, 0($sp)   #stores 0xdeadbeef @ 0x9000
  sw $t1, 16($sp)  #stores 0x0000beef @ 0x9010
  sw $t0, -16($sp) #stores 0xdead0000 @ 0x8FF0
  
  ori $v0, $0, 34
  or $a0, $0, $sp
  syscall          #should print 0xdeadbeef
  
  ori $v0, $0, 1
  
  lb $a0, 0($sp)
  syscall
  
  lb $a0, 1($sp)
  syscall
  
  lb $a0, 2($sp)
  syscall
  
  lb $a0, 3($sp)
  syscall
  
  #should print 0xdeadbeef byte by byte in integer form
  
  
  ori $t3, $0, 4096 # sets $t3 to 0x1000, start of code
  lw $t0, 0($t3)  
  ori $t3, $0, 8192 # sets $t3 to 0x2000
  lw $t1, 0($t3)
  sw $t0, 0($sp)
  
  ori $v0, $0, 34
  or $a0, $0, $sp
  syscall          #should print first instruction in hex
  
  sw $t0, 0($t3) # should print BAD WRITE @ 0x2000


# exit the simulation (v0 = 10, syscall)
	ori   $v0, $zero, 10
	syscall

.end _start

.data

