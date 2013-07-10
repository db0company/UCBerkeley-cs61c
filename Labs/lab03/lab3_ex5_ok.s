          .data
source:   .word   3, 1, 4, 1, 5, 9, 0
dest:     .word   0, 0, 0, 0, 0, 0, 0
countmsg: .asciiz " values copied. "

          .text

main:   la      $a0,source	# a0 = &source
        la      $a1,dest  	# a1 = &dest
	addi	$v0, $0, 0	#### v0 = 0

loop:   lw      $v1, 0($a0)     # v1 = source[0] = 3	# read next word from source
        addiu   $v0, $v0, 1     # v0++  = 1		# increment count words copied
        sw      $v1, 0($a1)     # dest[0] = v1 = 3	# write to destination
        addiu   $a0, $a0, 4     #### a0++ = &source[1]	# advance pointer to next source
        addiu   $a1, $a1, 4     #### a1++ = &dest[1]	# advance pointer to next dest
        bne     $v1, $zero, loop# if v1 goto loop	# loop if word copied not zero

loopend: # do we need this?
	addi	$v0, $v0, -1	### we need to decrement it because we increment at the beginning of the loop so it's one value more
	move    $a0,$v0         # a0 = v0		# $a0 <- count
        jal     puti            #			# print it

        la      $a0,countmsg    # a0 = countmsg		# $a0 <- countmsg
        jal     puts            #			# print it

        li      $a0,0x0A        #			# $a0 <- '\n'
        jal     putc            #			# print it

finish:
        li      $v0, 10         #			# Exit the program
        syscall




### The following functions do syscalls in order to print data (integer, string, character)
#Note: argument $a0 to syscall has already been set by the CALLEE

puti:
        li      $v0, 1          # specify Print Integer service
        syscall                 # Print it
        jr      $ra             # Return

puts:
        li      $v0, 4          # specify Print String service
        syscall                 # Print it
        jr      $ra             # Return

putc:
        li      $v0, 11         # specify Print Character service
        syscall                 # Print it
        jr      $ra             # Return
