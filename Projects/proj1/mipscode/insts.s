.data
passstring:     .asciiz "all tests passed!\n"
fail1string:    .asciiz "test #"
fail2string:    .asciiz " failed!\n"

.text

# _start is the entry point into any program.
.global _start
.ent    _start 
_start:

#
#  The header ends here, and code goes below
#

        # test #1: beq
        ori   $2, $0, 1
        ori   $3, $0, 1
        beq   $2, $3, 1f

        ori   $30, 1
        j     fail
1:      

        # test #2: bne
        ori   $2, $0, 1
        ori   $3, $0, 1
        bne   $2, $3, 1f
        j     2f

1:      ori   $30, 2
        j     fail
2:      

        # test #3: sll
        li    $30, 3
        li    $3, 1024
        sll   $4, $3, 2
        li    $5, 4096
        bne   $4, $5, fail

        # test #4: srl
        li    $30, 4
        li    $3, 1024
        srl   $4, $3, 2
        li    $5, 256
        bne   $4, $5, fail

        # test #5: addu
        li    $30, 5
        li    $3, 1234
        li    $5, 5678
        addu  $4, $3, $5
        li    $5, 6912
        bne   $4, $5, fail

        # test #6: and
        li    $30, 6
        li    $3, 1234
        li    $5, 5678
        and   $4, $3, $5
        li    $5, 1026
        bne   $4, $5, fail

        # test #7: slt
        li    $30, 7
        li    $3, 1234
        li    $5, 5678
        slt   $4, $3, $5
        li    $5, 1
        bne   $4, $5, fail

        # test #8: addiu
        li    $30, 8
        li    $3, 1234
        addiu $4, $3, 5678
        li    $5, 6912
        bne   $4, $5, fail

        # test #9: lui
        li    $30, 9
        lui   $4, 1234
        li    $5, 1234
        sll   $5, $5, 16
        bne   $4, $5, fail

        # test #10: lw
        li    $30, 10
        la    $4, foo
        lw    $4, 0($4)
        li    $5, 911
        bne   $4, $5, fail

        # test #11: sw
        li    $30, 11
        la    $4, foo
        li    $5, 912
	 addiu $4, $4, 28672
	 addiu $4, $4, 28672
        sw    $5, 0($4)
        lw    $4, 0($4)
        bne   $4, $5, fail

        # test #12: jal
        li    $30, 12
        li    $4, 0
        li    $5, 911
        jal   1f
2:      bne   $4, $5, fail
        la    $4, 2b
        bne   $4, $31, fail
        j 2f
1:      li    $4, 911
        j     2b
2:

pass:
        la $a0, passstring
        li $v0, 4
        syscall
        b done

fail:
        ori $a0, $0, %lo(fail1string)
        ori $v0, $0, 4
        syscall

        ori  $a0, $30, 0
        ori  $v0, $0, 1
        syscall

        ori $a0, $0, %lo(fail2string)
        ori $v0, $0, 4
        syscall

done:
        ori $v0, $zero, 10
        syscall

.end _start

.data
foo: .word 911
