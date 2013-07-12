#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "memory.h"

/* ***************************************************** */
/*                      GLOBAL STUFF                     */
/* ***************************************************** */

static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}

/* ***************************************************** */
/*                       LIST STUFF                      */
/* ***************************************************** */

typedef struct s_item {
  void * data;
  struct s_item * next;
  struct s_item * prev;
} item;

typedef struct {
  item * begin;
  size_t size;
} list;

static item * list_new_item(void * data) {
  item * new;
  if (!(new = malloc(sizeof(*new))))
    allocation_failed();
  new->data = data;
  return new;
}

/* Insert a new element to a list.
 * Create a new list if the list pointer is NULL */

list *	list_insert(list * l, void * data) {
  item * new = list_new_item(data);
  if (!l) {
    if (!(l = malloc(sizeof(*l))))
      allocation_failed();
    l->begin = new;
    l->size = 1;
    new->next = new;
    new->prev = new;
  } else {
    new->next = l->begin;
    l->begin->prev->next = new;
    new->prev = l->begin->prev;
    l->begin->prev = new;
    ++(l->size);
  }
  return l;
}

/* Insert an element containing data right after the element item
 */
void list_insert_after(list * l, item * i, void * data) {
  item * n;
  if (!l || !i)
    return ;
  n = list_new_item(data);
  i->next->prev = n;
  n->next = i->next;
  i->next = n;
  n->prev = i;
}

/* Apply a function to every item of a list.
 * The function should cast the item to the corresponding type.
 * A param is given to the function. If you don't want to use it, give NULL.
 */
void	list_foreach(list * list, void (* f)(void *, void *), void * param) {
  item * curr;
  size_t i;
  if (!list)
    return ;
  curr = list->begin;
  for (i = 0; i < list->size; ++i, curr = curr->next)
    f(curr->data, param);
}

/* Apply a printer function to every item of the list.
 */
void	list_print(list * list, void (* printer)(void *, void *), void * param) {
  list_foreach(list, printer, param);
}

/* Apply a function to every items of the list, return the first that
 * return true, as the item or just a data.
 * A param is given to the function. If you don't want to use it, give NULL.
*/
item *	list_find_item(list * list, bool (* f)(void *, void *), void * param) {
  item * curr;
  size_t i;
  if (!list)
    return NULL;
  curr = list->begin;
  for (i = 0; i < list->size; ++i, curr = curr->next)
    if (f(curr->data, param))
      return curr;
  return NULL;
}

void *	list_find(list * list, bool (* f)(void *, void *), void * param) {
  item * r = list_find_item(list, f, param);
  return r ? r->data : NULL;
}

/* Delete an entire list.
 * Apply a deleter function to the items before deleting them.
 * You can give "free" if you just allocated the data.
 * You can give NULL if you don't need to do anything before deleting.
 */
void	list_delete(list * list, void (*deleter)(void *)) {
  item * curr, * next;
  size_t i;
  if (!list)
    return ;
  curr = list->begin;
  for (i = 0; i < list->size; ++i) {
    if (deleter)
      deleter(curr->data);
    next = curr->next;
    free(curr);
    curr = next;
  }
  free(list);
}

/* ***************************************************** */
/*                    MEMORY STUFF                       */
/* ***************************************************** */


/* Pointer to simulator memory */
uint8_t * mem;
/* Heap */
list * heap;
/* Last address allocated in the stack */
uint32_t last_address;

typedef struct {
  bool used;
  uint32_t mipsaddr;
  uint32_t size;
} block;

/* ***************************************************** */
/*                   Heap Getters                        */
/* ***************************************************** */

/* if !mipsaddr, any block with enough size will match */
/* if !size, any size will match */
static bool aux_find_in_heap(void * va, void * vp) {
  block * a = va;
  block * p = vp;
  if (!p->size)
    return (a->used == p->used
	    && p->mipsaddr >= a->mipsaddr);
  if (!p->mipsaddr)
    return (a->used == p->used
	    && a->size <= p->size);
  return (a->used == p->used
	  && p->mipsaddr >= a->mipsaddr
	  && p->mipsaddr + p->size < a->mipsaddr + a->size);
}

static item * find_item_in_heap_from_block(block * block) {
  if (!block->size && !block->mipsaddr)
    return NULL;
  return list_find(heap, aux_find_in_heap, block);
}

static item * find_item_in_heap(uint32_t mipsaddr, uint32_t size, bool used) {
  block i;
  i.used = used;
  i.mipsaddr = mipsaddr;
  i.size = size;
  return find_item_in_heap_from_block(&i);
}

/* static block * find_block_in_heap(uint32_t mipsaddr, uint32_t size, bool used) { */
/*   return (find_item_in_heap(mipsaddr, size, used))->data; */
/* } */

static item * find_available_item_in_heap(uint32_t size) {
  return find_item_in_heap(0, size, false);
}

/* static item * find_available_block_in_heap(uint32_t size) { */
/*   return (find_available_item_in_heap(size))->data; */
/* } */

static item * find_allocated_item_in_heap(uint32_t mipsaddr, uint32_t size) {
  return find_item_in_heap(mipsaddr, size, false);
}

static block * find_allocated_block_in_heap(uint32_t mipsaddr, uint32_t size) {
  return (find_allocated_item_in_heap(mipsaddr, size))->data;
}

static block * find_allocated_block_in_heap_any_size(uint32_t mipsaddr) {
  return find_allocated_block_in_heap(mipsaddr, 0);
}

/* ***************************************************** */
/*                        Tools                          */
/* ***************************************************** */

/* Called by program loader to initialize memory. */
uint8_t* init_mem() {
  assert (mem == NULL);
  mem = (uint8_t*) calloc(MEM_SIZE, sizeof(uint8_t)); // allocate zeroed memory
  if (mem == NULL)
    fprintf(stderr, "%s: Allocation Failed\n", __FUNCTION__);
  heap = NULL;
  last_address = CODE_TOP;
  return (uint8_t*) mem;
}

/* Check if any part of the memory that you try to access is in the range, inclusive - exclusive */
static int check_range_ie(uint32_t mipsaddr, mem_unit_t size, uint32_t min, uint32_t max) {
  return mipsaddr >= min && (mipsaddr + size) < max;
}

/* Returns 1 if memory access is ok, otherwise returns 0. 
   The parameter write_permission is the macro READ if this is
   a read request, and WRITE if it is a write request. See memory.h. */
int access_ok(uint32_t mipsaddr, mem_unit_t size, uint8_t write_permission) {
  return
    !(
      /* Access RESERVED */
      check_range_ie(mipsaddr, size, 0, CODE_BOTTOM)

      /* Write on a READ-ONLY */
      || (write_permission == WRITE
	  && check_range_ie(mipsaddr, size, CODE_BOTTOM, CODE_TOP))

      /* Access unallocated on HEAP */
      || (check_range_ie(mipsaddr, size, HEAP_BOTTOM, HEAP_TOP)
	  && find_allocated_block_in_heap(mipsaddr, size))

      /* Aligned adress */
      || (mipsaddr % size)

      /* Above MEM_SIZE */
      || ((mipsaddr + size) >= MEM_SIZE)
      );
}

/* Writes size bytes of value into mips memory at mipsaddr */
void store_mem(uint32_t mipsaddr, mem_unit_t size, uint32_t value) {
  if (!access_ok(mipsaddr, size, WRITE)) {
    fprintf(stderr, "%s: bad write=%08x\n", __FUNCTION__, mipsaddr);
    exit(-1);
  }
  memcpy(mem + mipsaddr, &value, size);
}

/* Returns zero-extended value from mips memory */
uint32_t load_mem(uint32_t mipsaddr, mem_unit_t size) {
  uint32_t result = 0;
  if (!access_ok(mipsaddr, size, READ)) {
    fprintf(stderr, "%s: bad read=%08x\n", __FUNCTION__, mipsaddr);
    exit(-1);
  }
  memcpy(&result, mem + mipsaddr, size);
  return result;
}

static block * new_block(bool used, uint32_t mipsaddr, uint32_t size) {
  block * new;
  if (!(new = malloc(sizeof(*new))))
    allocation_failed();
  new->used = used;
  new->mipsaddr = mipsaddr;
  new->size = size;
  return new;
}

static block * use_this_item(item * this_item, uint32_t size) {
  block * this_block = this_item->data;
  if (this_block->size == size) {
    this_block->used = true;
    return this_block;
  }
  list_insert_after(heap, this_item,
		    new_block(false,
			      this_block->mipsaddr + size,
			      this_block->size - size));
  this_block->size = size;
  return this_block;
}

static block * allocate_new_block(uint32_t size) {
  void * memory;
  block * result;
  if (!(memory = malloc(size)))
    allocation_failed();
  heap = list_insert(heap, (result = new_block(true, (uint32_t)memory, size)));
  return result;
}

/*Uses the first fit algorithm to allocate a block of size bytes 
in the heap. Returns the address of the start of the block 
if allocation is successful, returns 0 on failure.*/
uint32_t first_fit_malloc(uint32_t size){
  item * available;
  if (!size)
    return 0;
  return
    ((available = find_available_item_in_heap(size)) ?
     use_this_item(available, size)
     : allocate_new_block(size))->mipsaddr;
}

/*Free the allocated block of memory in the heap beginning at
mipsaddr. In order to successfully free a block, mipsaddr must 
be the beginning of a block that was allocated on the heap. 
If block_free() is called on an unallocated memory address or an address
that is not the beginning of a block, bad_free() should be called.
*/

void block_free(uint32_t mipsaddr){
  block * b;
  if (!(b = find_allocated_block_in_heap_any_size(mipsaddr)))
    bad_free(mipsaddr);
  b->used = false;
}

/*Implementation of this function is optional but may be useful for debugging.
  This function will not be graded and is for your benefit only. To call this 
  function in MIPS, use syscall with $v0=64.*/

static void aux_print_heap_status(void * vb, void * _) {
  block * b = vb;
  printf("  0x%05x\t%s\t%d\n",
	 b->mipsaddr,
	 b->used ? "    " : "free",
	 b->size);
}

void print_heap_status() {
  printf("Heap dump:\n");
  list_print(heap, aux_print_heap_status, NULL);
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


