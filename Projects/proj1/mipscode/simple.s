.text

# _start is the entry point into any program.
.global _start
.ent    _start 
_start:

#
#  The header ends here, and code goes below
#

# print a string.  (note the %lo syntax only works if the string is
# in the lower 32KB of memory, which it always will be for us)
	ori   $a0, %lo(str)
	ori   $v0, $zero, 4
	syscall

# exit the simulation (v0 = 10, syscall)
	ori   $v0, $zero, 10
	syscall

.end _start

.data
str: .asciiz "Hello, world!\n"
