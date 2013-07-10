#ifndef __MIPS_MEM_H
#define __MIPS_MEM_H

#include <stdint.h>


typedef enum {
  SIZE_BYTE = 1,
  SIZE_HALF_WORD = 2,
  SIZE_WORD = 4,
} mem_unit_t;

#define MEM_SIZE (64*1024) /* 16 bit address space, 64 Kibibytes of memory*/
#define CODE_BOTTOM (0x1000) /* [0x0000,0x0FFF] is reserved (inaccessible)*/
#define CODE_TOP (0x4000) /* [0x1000,0x3FFF] is code section (read-only) */
#define HEAP_BOTTOM CODE_TOP /*CODE_TOP = HEAP_BOTTOM , first valid heap address */
#define HEAP_TOP (0x8000) /* heap is [0x4000,0x7FFF] */ 
#define STACK_ORIGIN (MEM_SIZE-4) /*points to largest word-aligned address*/
#define WRITE 1
#define READ 0


uint8_t *init_mem();
void store_mem(uint32_t mipsaddr, mem_unit_t size, uint32_t value);
uint32_t load_mem(uint32_t mipsaddr, mem_unit_t size);


//NEW FUNCTIONS
uint8_t *init_heap_status();
uint32_t first_fit_malloc(uint32_t size);
void block_free(uint32_t mipsaddr);
void print_heap_status();
void bad_free(uint32_t mipsaddr);
uint32_t next_fit_malloc(uint32_t size);


#endif // __PROCESSOR_H
