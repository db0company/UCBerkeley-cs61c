#ifndef __PROCESSOR_H
#define __PROCESSOR_H

#include <stdint.h>
#include "memory.h"

typedef uint32_t reg_t;

typedef struct
{
  unsigned int funct : 6;
  unsigned int shamt : 5;
  unsigned int rd : 5;
  unsigned int rt : 5;
  unsigned int rs : 5;
  unsigned int opcode : 6;
} rtype_inst_t;

typedef struct
{
  unsigned int imm : 16;
  unsigned int rt : 5;
  unsigned int rs : 5;
  unsigned int opcode : 6;
} itype_inst_t;

typedef struct
{
  unsigned int addr : 26;
  unsigned int opcode : 6;
} jtype_inst_t;

typedef union
{
  rtype_inst_t rtype;
  itype_inst_t itype;
  jtype_inst_t jtype;
  int16_t chunks16[2];
  uint32_t bits;
} inst_t;

typedef struct
{
  reg_t R[32];  
  reg_t pc;
} processor_t;

void execute_one_inst(processor_t* p, int prompt, int print_regs);
void init_processor(processor_t* p);
void print_registers(processor_t* p);
void handle_syscall(processor_t* p);
void handle_arith_overflow(processor_t* p);
#endif // __PROCESSOR_H
