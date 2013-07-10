#include <stdio.h>
#include <stdlib.h>

#include "disassemble.h"

void disassemble(inst_t inst)
{
  /* you could use itype or type instead of rtype */
  /* remember that opcode field is at the same place for all types */
  switch (inst.rtype.opcode)
  {
  case 0x0: // opcode == 0x0 (SPECIAL)

    switch (inst.rtype.funct)
    {
    case 0xc: // funct == 0xc (SYSCALL)
      printf("syscall\n");
      break;
    
    case 0x24: // funct == 0x24 (AND)
      printf("and\t$%d,$%d,$%d\n", inst.rtype.rd, inst.rtype.rs, inst.rtype.rt);
      break;      

    case 0x25: // funct == 0x25 (OR)
      printf("or\t$%d,$%d,$%d\n", inst.rtype.rd, inst.rtype.rs, inst.rtype.rt);
      break;

    default: // undefined funct
      fprintf(stderr, "%s: illegal instruction: %08x\n", __FUNCTION__, inst.bits);
      exit(-1);
      break;
    }
    break;

  case 0xd: // opcode == 0xd (ORI)
    printf("ori\t$%d,$%d,0x%x\n", inst.itype.rt, inst.itype.rs, inst.itype.imm);
    break;

  case 0x2: // opcode == 0x2 (J)
    printf("j\t0x%x\n", inst.jtype.addr*4);
    break;

  default: // undefined opcode
    fprintf(stderr, "%s: illegal instruction: %08x\n", __FUNCTION__, inst.bits);
    exit(-1);
    break;
  }
}
