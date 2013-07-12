#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "processor.h"
#include "disassemble.h"

void execute_one_inst(processor_t* p, int prompt, int print_regs)
{
  inst_t inst;

  /* fetch an instruction */
  inst.bits = load_mem(p->pc, SIZE_WORD);

  /* interactive-mode prompt */
  if(prompt)
  {
    if (prompt == 1) {
      printf("simulator paused, enter to continue...");
      while(getchar() != '\n')
        ;
    }
    printf("%08x: ",p->pc);
    disassemble(inst);
  }

  switch (inst.rtype.opcode) /* could also use e.g. inst.itype.opcode */
  {
  case 0x0: // opcode == 0x0 (SPECIAL)

    switch (inst.rtype.funct)
    {
    case 0xc: // funct == 0xc (SYSCALL)
      handle_syscall(p);
      p->pc += 4;
      break;
      
    case 0x24: // funct == 0x24 (AND)
      p->R[inst.rtype.rd] = p->R[inst.rtype.rs] & p->R[inst.rtype.rt];
      p->pc += 4;
      break;      

    case 0x25: // funct == 0x25 (OR)
      p->R[inst.rtype.rd] = p->R[inst.rtype.rs] | p->R[inst.rtype.rt];
      p->pc += 4;
      break;

    default: // undefined funct
      fprintf(stderr, "%s: pc=%08x, illegal instruction=%08x\n", __FUNCTION__, p->pc, inst.bits);
      exit(-1);
      break;
    }
    break;

  case 0xd: // opcode == 0xd (ORI)
    p->R[inst.itype.rt] = p->R[inst.itype.rs] | inst.itype.imm;
    p->pc += 4;
    break;

  case 0x2: // opcode == 0x2 (J)
    p->pc = ((p->pc+4) & 0xf0000000) | (inst.jtype.addr << 2);
    break;

  default: // undefined opcode
    fprintf(stderr, "%s: pc=%08x, illegal instruction: %08x\n", __FUNCTION__, p->pc, inst.bits);
    exit(-1);
    break;
  }

  // enforce $0 being hard-wired to 0
  p->R[0] = 0;

  if(print_regs)
    print_registers(p);
}

void init_processor(processor_t* p)
{
  int i;

  /* initialize pc to 0x1000 */
  p->pc = 0x1000;

  /* zero out all registers */
  for (i=0; i<32; i++)
  {
    p->R[i] = 0;
  }
  /* set stack pointer to top-most word in memory*/
  p->R[29]=STACK_ORIGIN;
}

void print_registers(processor_t* p)
{
  int i,j;
  for (i=0; i<8; i++)
  {
    for (j=0; j<4; j++)
      printf("r%2d=%08x ",i*4+j,p->R[i*4+j]);
    puts("");
  }
}

void handle_arith_overflow(processor_t* p){
	printf("Arithmetic Overflow Exception at 0x%08x\n",p->pc);
    exit(0);
}


void handle_syscall(processor_t* p)
{
  reg_t i;

  switch (p->R[2]) // syscall number is given by $v0 ($2)
  {
  case 1: // print an integer
    printf("%d", p->R[4]);
    break;

  case 4: // print a string
    for(i = p->R[4]; i < MEM_SIZE && load_mem(i, SIZE_BYTE); i++)
      printf("%c", load_mem(i, SIZE_BYTE));
    break;

  case 10: // exit
    printf("exiting the simulator\n");
    destroy_mem();
    exit(0);
    break;

  case 11: // print a character
    printf("%c", p->R[4]);
    break;
    
  case 34: //print the contents of a word aligned address in hex
    printf("0x%x\n", load_mem(p->R[4],SIZE_WORD));
    break;
  
  case 60: // next fit malloc
    p->R[2] = next_fit_malloc(p->R[4]);
    break;    
  
  case 61: // first fit malloc
    p->R[2] = first_fit_malloc(p->R[4]);
    break;
    
  case 62: // free heap block
    block_free(p->R[4]);
    break;
    
  case 63: //print heap allocation status
    print_heap_status();
    break;

  default: // undefined syscall
    fprintf(stderr, "%s: illegal syscall number %d\n", __FUNCTION__, p->R[2]);
    exit(-1);
    break;
  }
}
