#include <stdio.h>
#include <stdlib.h>

#include "disassemble.h"

void illegal(inst_t inst) {
  fprintf(stderr, "%s: illegal instruction: %08x\n", __FUNCTION__, inst.rtype.opcode);
  exit(-1);
}

typedef struct {
  unsigned int opcode : 6;
  char * name;
  void (*printer)(char *, inst_t);
} optos_t;

void r(char * name, inst_t inst) {
  printf("%s\t$%d,$%d,$%d\n", name, inst.rtype.rd, inst.rtype.rs, inst.rtype.rt);
}

void r_shamt(char * name, inst_t inst) {
  printf("%s\t$%d,$%d,%d\n", name, inst.rtype.rd, inst.rtype.rt, inst.rtype.shamt);
}

void r_rs(char * name, inst_t inst) {
  printf("%s\t$%d\n", name, inst.rtype.rs);
}

void i_offset(char * name, inst_t inst) {
  printf("%s\t$%d,$%d,%d\n", name, inst.itype.rs, inst.itype.rt, inst.itype.imm * 4);
}

void i_rt_offset(char * name, inst_t inst) {
  printf("%s\t$%d,%d($%d)\n", name, inst.itype.rt, inst.itype.imm, inst.itype.rs);
}

void i_rtrs(char * name, inst_t inst) {
  printf("%s\t$%d,$%d,%d\n", name, inst.itype.rt, inst.itype.rs, inst.itype.imm);
}

void i_rs_hex(char * name, inst_t inst) {
  printf("%s\t$%d,0x%x\n", name, inst.itype.rs, inst.itype.imm);
}

void i_rt_hex(char * name, inst_t inst) {
  printf("%s\t$%d,0x%x\n", name, inst.itype.rt, inst.itype.imm);
}

void i_rtrs_hex(char * name, inst_t inst) {
  printf("%s\t$%d,$%d,0x%x\n", name, inst.itype.rt, inst.itype.rs, inst.itype.imm);
}

void j(char * name, inst_t inst) {
  printf("%s\t0x%x\n", name, inst.jtype.addr * 4);
}

void just_name(char * name, inst_t _) {
  printf("%s\n", name);
}

optos_t special[] = {
  {0x0, "sll", r_shamt},
  {0x2, "srl", r_shamt},
  {0x3, "sra", r_shamt},
  {0x8, "jr", r_rs},
  {0x9, "jalr", r_rs},
  {0xc, "syscall", just_name},
  {0x20, "add", r},
  {0x21, "addu", r},
  {0x24, "and", r},
  {0x25, "or", r},
  {0x26, "xor", r},
  {0x2a, "slt", r},
};
size_t special_size = sizeof(special) / sizeof(*special);

void s(char * _, inst_t inst) {
  ssize_t i;
  for (i = 0; i < special_size; ++i)
    if (special[i].opcode == inst.rtype.funct) {
      special[i].printer(special[i].name, inst);
      return ;
    }
  illegal(inst);
}

optos_t optos[] = {
  {0x0, "special", s},
  {0xd, "ori", i_rtrs_hex},
  {0x2, "j", j},
  {0x3, "jal", j},
  {0x4, "beq", i_offset},
  {0x5, "bne", i_offset},
  {0x9, "addiu", i_rtrs},
  {0xe, "xori", i_rtrs},
  {0xf, "lui", i_rt_hex},
  {0x20, "lb", i_rt_offset},
  {0x23, "lw", i_rt_offset},
  {0x24, "lbu", i_rt_offset},
  {0x28, "sb", i_rt_offset},
  {0x2b, "sw", i_rt_offset},
};
size_t optos_size = sizeof(optos) / sizeof(*optos);

void disassemble(inst_t inst) {
  ssize_t i;
  for (i = 0; i < optos_size; ++i)
    if (optos[i].opcode == inst.rtype.opcode) {
      optos[i].printer(optos[i].name, inst);
      return ;
    }
  illegal(inst);
}
