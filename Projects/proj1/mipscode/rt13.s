.text
.set noat

.ent _start
.global _start
_start:

  ori $t0, $0, %lo(foo)
  jal $t0, $t0
  ori $t0, $0, 1

foo:
  ori $t0, $0, 2

  ori $v0, $0, 10
  syscall

.end _start
