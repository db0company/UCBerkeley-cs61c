
/* Compile me with:
 * gcc memory.c test.c
 * Test full with:
 * gcc -g -Wall memory.c test.c && valgrind ./a.out */

#include <stdio.h>
#include "memory.h"

int main(void) {
  uint32_t sp = 0x9000;
  char c;
  uint32_t m1, m2, m3, m4;

  init_mem();

  printf("### Values\n");

  printf("Stack pointer sp: 0x%05x\n", sp);
  printf("Reserved : 0x%05x - 0x%05x\n", 0, CODE_BOTTOM);
  printf("Code     : 0x%05x - 0x%05x\n", CODE_BOTTOM, CODE_TOP);
  printf("Heap     : 0x%05x - 0x%05x\n", CODE_TOP, HEAP_TOP);
  printf("Stack    : 0x%05x - 0x%05x\n", HEAP_TOP, MEM_SIZE);

  printf("\n");
  printf("### Tests on the stack\n");

  store_mem(sp, SIZE_WORD, 0xFFFFFF);
  store_mem(sp - 4, SIZE_BYTE, 42);
  store_mem(sp - 5, SIZE_BYTE, 43);
  store_mem(sp - 6, SIZE_BYTE, 44);
  store_mem(sp - 7, SIZE_BYTE, 45);
  store_mem(sp - 10, SIZE_HALF_WORD, 1024);

  printf("0x%x\n", load_mem(sp, SIZE_WORD));
  c = load_mem(sp - 4, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(sp - 5, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(sp - 6, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(sp - 7, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  printf("%d\n", load_mem(sp - 10, SIZE_HALF_WORD));

  printf("\n");
  printf("### Test on the code\n");

  load_mem(0x2000, SIZE_WORD);
  printf("OK\n");

  printf("\n");
  printf("### Tests on the heap\n");

  print_heap_status();
  m1 = first_fit_malloc(10);
  print_heap_status();
  m2 = first_fit_malloc(50);
  print_heap_status();
  m3 = first_fit_malloc(100);
  print_heap_status();
  m4 = first_fit_malloc(50);
  print_heap_status();

  store_mem(m1, SIZE_WORD, 0xFFFFFF);
  store_mem(m2, SIZE_BYTE, 42);
  store_mem(m2 + 1, SIZE_BYTE, 43);
  store_mem(m2 + 2, SIZE_BYTE, 44);
  store_mem(m2 + 3, SIZE_BYTE, 45);
  store_mem(m2 + 10, SIZE_HALF_WORD, 1024);

  printf("0x%x\n", load_mem(m1, SIZE_WORD));
  c = load_mem(m2, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(m2 + 1, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(m2 + 2, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  c = load_mem(m2 + 3, SIZE_BYTE);
  printf("%c (%d)\n", c, c);
  printf("%d\n", load_mem(m2 + 10, SIZE_HALF_WORD));

  block_free(m2);
  print_heap_status();
  m2 = first_fit_malloc(30);
  print_heap_status();
  block_free(m1);
  print_heap_status();
  block_free(m2);
  print_heap_status();
  block_free(m3);
  print_heap_status();
  block_free(m4);
  print_heap_status();
  m2 = first_fit_malloc(90);
  print_heap_status();
  block_free(m2);
  print_heap_status();


  destroy_mem();

  return 0;
}
