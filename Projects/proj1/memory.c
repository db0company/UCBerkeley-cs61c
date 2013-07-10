#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"


/* Pointer to simulator memory */
uint8_t *mem;
/* Create variable to keep track of heap status here 
   You may change this to use your own implementation. */
uint8_t *heap_status;


/* Called by program loader to initialize memory. */
uint8_t* init_mem() {
  assert (mem == NULL);
  mem = (uint8_t*) calloc(MEM_SIZE, sizeof(uint8_t)); // allocate zeroed memory
  if (mem==NULL) fprintf(stderr, "%s: Allocation Failed\n", __FUNCTION__);

  //you may change the following line if using your own implementation to maintain the heap
  heap_status = (uint8_t*) init_heap_status();
  return (uint8_t*) mem;
}

/* Called in init_mem() to initialize your implementation of
   how you keep track of heap allocation status.*/
uint8_t* init_heap_status(){

  /* TODO YOUR CODE HERE */
  
  return (uint8_t*) 0;
}

/* Returns 1 if memory access is ok, otherwise returns 0. 
The parameter write_permission is the macro READ if this is
a read request, and WRITE if it is a write request. See memory.h. */

int access_ok(uint32_t mipsaddr, mem_unit_t size, uint8_t write_permission) {

  /* TODO YOUR CODE HERE */
  
  
  
  return 1;
}

/* Writes size bytes of value into mips memory at mipsaddr */
void store_mem(uint32_t mipsaddr, mem_unit_t size, uint32_t value) {
  if (!access_ok(mipsaddr, size, WRITE)) {
    fprintf(stderr, "%s: bad write=%08x\n", __FUNCTION__, mipsaddr);
    exit(-1);
  }

  /* TODO YOUR CODE HERE */


}

/* Returns zero-extended value from mips memory */
uint32_t load_mem(uint32_t mipsaddr, mem_unit_t size) {
  if (!access_ok(mipsaddr, size, READ)) {
    fprintf(stderr, "%s: bad read=%08x\n", __FUNCTION__, mipsaddr);
    exit(-1);
  }

  /* TODO YOUR CODE HERE */

  // incomplete stub to let mipscode/simple execute
  // (only handles size == SIZE_WORD correctly)
  // feel free to delete and implement your own way
  
  return *(uint32_t*)(mem + mipsaddr);
}

/*Uses the first fit algorithm to allocate a block of size bytes 
in the heap. Returns the address of the start of the block 
if allocation is successful, returns 0 on failure.*/

uint32_t first_fit_malloc(uint32_t size){
  /* TODO YOUR CODE HERE */
  uint8_t *hs_pointer = heap_status;
  

  
  //NOT YET IMPLEMENTED, YOU MAY NEED TO CHANGE RETURN VALUE
  return HEAP_BOTTOM + (hs_pointer-heap_status);
}


/*Free the allocated block of memory in the heap beginning at
mipsaddr. In order to successfully free a block, mipsaddr must 
be the beginning of a block that was allocated on the heap. 
If block_free() is called on an unallocated memory address or an address
that is not the beginning of a block, bad_free() should be called.
*/

void block_free(uint32_t mipsaddr){
  /* TODO YOUR CODE HERE */
  
  
}



/*Implementation of this function is optional but may be useful for debugging.
  This function will not be graded and is for your benefit only. To call this 
  function in MIPS, use syscall with $v0=64.*/

void print_heap_status(){
  //YOUR CODE HERE
  //OPTIONAL FOR DEBUGGING
}

/*Called when trying to free an invalid address. Do NOT
  change this function.*/
void bad_free(uint32_t mipsaddr){
  fprintf(stderr, "%s: bad free=%08x\n", __FUNCTION__, mipsaddr);
  exit(-1);
}


/**Extra Credit**/
/*Wait until completing the rest of the project to work on this part; it
  requires significantly more work than first_fit_malloc().
  You may need to declare new variables and/or modify functions you 
  have already completed.*/


uint32_t next_fit_malloc(uint32_t size){
  /* TODO YOUR CODE HERE */
  return 0;
}


