#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "memory.h"
#include "processor.h"
#include "disassemble.h"

#define upbits(a, n) (a >> ((sizeof(a) * 8) - n))
#define nextpc (p->pc + SIZE_WORD)
#define signext(a) ((signed int)a)   /* C casts properly assign the remaining */
#define zeroext(a) ((unsigned int)a) /* bits for me :) */
#define signed(a) ((signed int)a)
#define ispos(a) (a >= 0)
#define isneg(a) (a <= 0)

#define Rrd p->R[inst.rtype.rd]
#define Rrt p->R[inst.rtype.rt]
#define Rrs p->R[inst.rtype.rs]
#define Rshamt p->R[inst.rtype.shamt]

#define Irt p->R[inst.itype.rt]
#define Irs p->R[inst.itype.rs]
#define Iimm inst.itype.imm
#define Ioffset Iimm

#define Jaddr inst.jtype.addr

static void illegal(processor_t * p, inst_t inst) {
  fprintf(stderr, "%s: pc=%08x, illegal instruction=%08x\n",
	  __FUNCTION__, p->pc, inst.bits);
  exit(-1);
}

static void sll(processor_t * p, inst_t inst) {
  Rrd = Rrs < Rrt ? 1 : 0;
}

static void srl(processor_t * p, inst_t inst) {
  Rrd = Rrt >> Rshamt;
}

static void sra(processor_t * p, inst_t inst) {
  Rrd = signed(Rrt) >> Rshamt;
}

static void jr(processor_t * p, inst_t inst) {
  p->pc = Rrs;
}

static void jalr(processor_t * p, inst_t inst) {
  reg_t tmp = nextpc;
  p->pc = Rrs;
  p->R[31] = tmp;
}

static void add(processor_t * p, inst_t inst) {
  Rrd = signed(Rrs) + signed(Rrt);
  /* check for signed overflow */
  if ((ispos(Rrs) && ispos(Rrt) && isneg(Rrd))
      || (isneg(Rrs) && isneg(Rrs) && ispos(Rrd)))
    handle_arith_overflow(p);
}

static void addu(processor_t * p, inst_t inst) {
  Rrd = Rrs + Rrt;
}

static void and(processor_t * p, inst_t inst) {
  Rrd = Rrs & Rrt;
}

static void or(processor_t * p, inst_t inst) {
  Rrd = Rrs | Rrt;
}

static void xor(processor_t * p, inst_t inst) {
  Rrd = Rrs ^ Rrt;
}

static void slt(processor_t * p, inst_t inst) {
  Rrd = signed(Rrs) < signed(Rrt);
}

static void ori(processor_t * p, inst_t inst) {
  Irt = Irs | Iimm;
}

static void j(processor_t * p, inst_t inst) {
  p->pc = (nextpc & 0xF0000000) | (Jaddr << 2);
}

static void jal(processor_t * p, inst_t inst) {
  p->R[31]  = nextpc;
  p->pc = (nextpc & 0xF0000000) | (Jaddr << 2);
}

static void beq(processor_t * p, inst_t inst) {
  p->pc =
    (Irs == Irt ?
     nextpc + (signext(Ioffset) * SIZE_WORD)
     : nextpc);
}

static void bne(processor_t * p, inst_t inst) {
  p->pc =
    (Irs != Irt ?
     nextpc + (signext(Ioffset) * SIZE_WORD)
     : nextpc);
}

static void addiu(processor_t * p, inst_t inst) {
  Irt = Irs + signext(Iimm);
}

static void xori(processor_t * p, inst_t inst) {
  Irt = Rrs ^ zeroext(Iimm);
}

static void lui(processor_t * p, inst_t inst) {
  Irt = Iimm << 16;
}

static void lb(processor_t * p, inst_t inst) {
  Irt = signext(load_mem(Irs + signext(Ioffset), SIZE_BYTE));
}

static void lw(processor_t * p, inst_t inst) {
  Irt = load_mem(Irs + signext(Ioffset), SIZE_WORD);
}

static void lbu(processor_t * p, inst_t inst) {
  Irt = zeroext(load_mem(Irs + signext(Ioffset), SIZE_BYTE));
}

static void sb(processor_t * p, inst_t inst) {
  store_mem(Irs + signext(Ioffset), SIZE_BYTE, Irt);
}

static void sw(processor_t * p, inst_t inst) {
  store_mem(Irs + signext(Ioffset), SIZE_WORD, Irt);
}

static void illegal_syscall(processor_t * p, inst_t inst) {
  fprintf(stderr, "%s: illegal syscall number %d\n",
	  __FUNCTION__, p->R[2]);
  exit(-1);
}

static void syscall_print_integer(processor_t * p, inst_t inst) {
  printf("%d", p->R[4]);
}

static void syscall_print_string(processor_t * p, inst_t inst) {
  reg_t i;
  for(i = p->R[4]; i < MEM_SIZE && load_mem(i, SIZE_BYTE); i++)
    printf("%c", load_mem(i, SIZE_BYTE));
}

static void syscall_exit(processor_t * p, inst_t inst) {
  printf("exiting the simulator\n");
  destroy_mem();
  exit(0);
}

static void syscall_print_character(processor_t * p, inst_t inst) {
  printf("%c", p->R[4]);
}

static void syscall_print_address(processor_t * p, inst_t inst) {
  printf("0x%x\n", load_mem(p->R[4], SIZE_WORD));
}

static void syscall_next_fit_malloc(processor_t * p, inst_t inst) {
  p->R[2] = next_fit_malloc(p->R[4]);
}

static void syscall_first_fit_malloc(processor_t * p, inst_t inst) {
  p->R[2] = first_fit_malloc(p->R[4]);
}

static void syscall_block_free(processor_t * p, inst_t inst) {
  block_free(p->R[4]);
}

static void syscall_print_heap(processor_t * p, inst_t inst) {
  print_heap_status();
}

typedef struct {
  reg_t nb;
  void (*exe)(processor_t *, inst_t);
} systor_t;

static const systor_t systor[] = {
  {1, syscall_print_integer},
  {4, syscall_print_string},
  {10, syscall_exit},
  {11, syscall_print_character},
  {34, syscall_print_address},
  {60, syscall_next_fit_malloc},
  {61, syscall_first_fit_malloc},
  {62, syscall_block_free},
  {63, syscall_print_heap},
};
static const size_t systor_size = sizeof(systor) / sizeof(*systor);

/* Syscalls are handled using another table of opcodes.
 * The 3rd argument (move_pc) is ignored. */
static void syscall(processor_t * p, inst_t inst) {
  ssize_t i;
  for (i = 0; i < systor_size; ++i)
    if (p->R[2] == systor[i].nb) {
      systor[i].exe(p, inst);
      return ;
    }
  illegal_syscall(p, inst);  
}

typedef struct {
  unsigned int opcode : 6;
  void (*exe)(processor_t *, inst_t);
  bool move_pc;
} optor_t;

static const optor_t special[] = {
  {0x0, sll, true},
  {0x2, srl, true},
  {0x3, sra, true},
  {0x8, jr, false},
  {0x9, jalr, false},
  {0xc, syscall, true},
  {0x20, add, true},
  {0x21, addu, true},
  {0x24, and, true},
  {0x25, or, true},
  {0x26, xor, true},
  {0x2a, slt, true},
};
static const size_t special_size = sizeof(special) / sizeof(*special);

/* This function browse the special table in case the opcode
 * is 0, which is not enough to determine the type of the
 * instruction, so you use the "funct" field. */
static void s(processor_t * p, inst_t inst) {
  ssize_t i;
  for (i = 0; i < special_size; ++i)
    if (special[i].opcode == inst.rtype.funct) {
      special[i].exe(p, inst);
      if (special[i].move_pc)
	p->pc = nextpc;
      return ;
    }
  illegal(p, inst);
}

static const optor_t optor[] = {
  {0x0, s, true},
  {0xd, ori, true},
  {0x2, j, false},
  {0x3, jal, false},
  {0x4, beq, false},
  {0x5, bne, false},
  {0x9, addiu, true},
  {0xe, xori, true},
  {0xf, lui, true},
  {0x20, lb, true},
  {0x23, lw, true},
  {0x24, lbu, true},
  {0x28, sb, true},
  {0x2b, sw, true},
};
static const size_t optor_size = sizeof(optor) / sizeof(*optor);

void execute_one_inst(processor_t * p, int prompt, int print_regs) {
  inst_t inst;
  ssize_t i;

  /* fetch an instruction */
  inst.bits = load_mem(p->pc, SIZE_WORD);

  /* interactive-mode prompt */
  if (prompt) {
    if (prompt == 1) {
      printf("simulator paused, enter to continue...");
      while(getchar() != '\n');
    }
    printf("%08x: ", p->pc);
    disassemble(inst);
  }

  for (i = 0; i < optor_size; ++i)
    if (optor[i].opcode == inst.rtype.opcode) {
      optor[i].exe(p, inst);
      if (inst.rtype.opcode /* not special, cause it's been handled already */
	  && optor[i].move_pc) /* do I need to move the pointer? */
	p->pc = nextpc;
      goto end;
    }
  illegal(p, inst);

 end:
  // enforce $0 being hard-wired to 0
  p->R[0] = 0;

  if (print_regs)
    print_registers(p);
}

void init_processor(processor_t * p) {
  int i;

  /* initialize pc to 0x1000 */
  p->pc = 0x1000;

  /* zero out all registers */
  for (i = 0; i < 32; i++) {
    p->R[i] = 0;
  }
  /* set stack pointer to top-most word in memory*/
  p->R[29] = STACK_ORIGIN;
}

void print_registers(processor_t * p) {
  int i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 4; j++)
      printf("r%2d=%08x ", (i * 4) + j, p->R[(i * 4) + j]);
    puts("");
  }
}

void handle_arith_overflow(processor_t * p) {
  printf("Arithmetic Overflow Exception at 0x%08x\n", p->pc);
  exit(0);
}
