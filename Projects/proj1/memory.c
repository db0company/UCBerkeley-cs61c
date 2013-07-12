#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "memory.h"

/* ***************************************************** */
/* ERRORS HANDLING                                       */
/* ***************************************************** */

static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}

static void bad_free(uint32_t mipsaddr){
  fprintf(stderr, "%s: bad free=%08x\n", __FUNCTION__, mipsaddr);
  exit(-1);
}

/* ***************************************************** */
/* Generic LIST functions                                */
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

/* Insert an element containing data right after the element item */
void list_insert_after(list * l, item * i, void * data) {
  item * n;
  if (!l || !i)
    return ;
  n = list_new_item(data);
  i->next->prev = n;
  n->next = i->next;
  i->next = n;
  n->prev = i;
  ++(l->size);
}

/* Apply a function to every item of a list.
 * The function should cast the item to the corresponding type.
 * A param is given to the function. If you don't want to use it, give NULL. */
void	list_foreach_item(list * list, void (* f)(item *, void *),
			  void * param) {
  item * curr;
  size_t i;
  if (!list)
    return ;
  curr = list->begin;
  for (i = 0; i < list->size; ++i, curr = curr->next)
    f(curr, param);
}
void	list_foreach(list * list, void (* f)(void *, void *), void * param) {
  item * curr;
  size_t i;
  if (!list)
    return ;
  curr = list->begin;
  for (i = 0; i < list->size; ++i, curr = curr->next)
    f(curr->data, param);
}

/* Apply a printer function to every item of the list. */
void	list_print(list * list, void (* printer)(void *, void *), void * param) {
  list_foreach(list, printer, param);
}

/* Apply a function to every items of the list, return the first that
 * return true, as the item or just a data.
 * A param is given to the function. If you don't want to use it, give NULL. */
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
 * You can give NULL if you don't need to do anything before deleting. */
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

/* Delete the given element from the list.
 * Apply a deleter function to the item before deleting it.
 * You can give "free" if you just allocated the data.
 * You can give NULL if you don't need to do anything before deleting. */
void list_delete_item(list * l, item * i, void (*deleter)(void *)) {
  if (!l)
    return ;
  if (l->size == 1) {
    list_delete(l, deleter);
    return ;
  }
  i->next->prev = i->prev;
  i->prev->next = i->next;
  if (deleter)
    deleter(i->data);
  free(i);
  --(l->size);
}

/* ***************************************************** */
/* MEMORY DATA                                           */
/* ***************************************************** */

/* Pointer to simulator memory */
uint8_t * mem;

/* Heap */
list * heap;

/* Last address allocated in the heap */
uint32_t last_address;

/* Block wrapper with info */
typedef struct {
  bool used;
  uint32_t mipsaddr;
  uint32_t size;
} block;

/* ***************************************************** */
/* Init/Destroy                                          */
/* ***************************************************** */

/* Called by program loader to initialize memory. */
uint8_t* init_mem() {
  assert (mem == NULL);
  mem = (uint8_t*) calloc(MEM_SIZE, sizeof(uint8_t)); /* allocate zeroed memory */
  if (mem == NULL)
    fprintf(stderr, "%s: Allocation Failed\n", __FUNCTION__);
  heap = NULL;
  last_address = HEAP_BOTTOM;
  return (uint8_t*) mem;
}

/* Should be called by program loader to destroy memory at the end */
void destroy_mem() {
  list_delete(heap, free);
  free(mem);
}

/* ***************************************************** */
/* Debug                                                 */
/* ***************************************************** */

static void aux_print_heap_status(item * i, __attribute__((unused)) void * _) {
  block * b;
  b = i->data;
  printf("  0x%05x\tto 0x%05x\t%d\t%s\n",
	 b->mipsaddr,
	 b->mipsaddr + b->size,
	 b->size,
	 b->used ? "" : "free"
	 );
}

/* Implementation of this function is optional but may be useful for debugging.
 * This function will not be graded and is for your benefit only. To call this 
 * function in MIPS, use syscall with $v0=64. */
void print_heap_status() {
  printf("\nHeap dump:\n");
  list_foreach_item(heap, aux_print_heap_status, NULL);
  printf("\n");
}

/* ***************************************************** */
/* Heap Getters                                          */
/* ***************************************************** */

/* if !mipsaddr, any block with enough size will match
 * if !size, any size will match */
static bool aux_find_in_heap(void * va, void * vp) {
  block * a = va;
  block * p = vp;
  if (!p->size) {
    return (a->used == p->used
	    && p->mipsaddr == a->mipsaddr);
  }
  if (!p->mipsaddr) {
    return (a->used == p->used
	    && a->size >= p->size);
  }
  return (a->used == p->used
	  && p->mipsaddr >= a->mipsaddr
	  && p->mipsaddr + p->size < a->mipsaddr + a->size);
}

static item * find_item_in_heap_from_block(block * block) {
  if (!block->size && !block->mipsaddr)
    return NULL;
  return list_find_item(heap, aux_find_in_heap, block);
}

static item * find_item_in_heap(uint32_t mipsaddr, uint32_t size, bool used) {
  block i;
  i.used = used;
  i.mipsaddr = mipsaddr;
  i.size = size;
  return find_item_in_heap_from_block(&i);
}

static item * find_available_item_in_heap(uint32_t size) {
  return find_item_in_heap(0, size, false);
}

static item * find_allocated_item_in_heap(uint32_t mipsaddr, uint32_t size) {
  return find_item_in_heap(mipsaddr, size, true);
}

static block * find_allocated_block_in_heap(uint32_t mipsaddr, uint32_t size) {
  item * b = find_allocated_item_in_heap(mipsaddr, size);
  return b ? b->data : NULL;
}

static block * find_allocated_block_in_heap_any_size(uint32_t mipsaddr) {
  return find_allocated_block_in_heap(mipsaddr, 0);
}

/* ***************************************************** */
/* Check access in virtual memory                        */
/* ***************************************************** */

/* Check if any part of the memory that you try to access is in the range,
 * inclusive - exclusive */
static int check_range_ie(uint32_t mipsaddr, mem_unit_t size, uint32_t min, uint32_t max) {
  return mipsaddr >= min && (mipsaddr + size) < max;
}

/* Returns 1 if memory access is ok, otherwise returns 0. 
 * The parameter write_permission is the macro READ if this is
 * a read request, and WRITE if it is a write request. See memory.h. */
int access_ok(uint32_t mipsaddr, mem_unit_t size, uint8_t write_permission) {
  return
    !(
      /* Access RESERVED */
      check_range_ie(mipsaddr, size, 0, CODE_BOTTOM)

      /* Write on the READ-ONLY section CODE */
      || (write_permission == WRITE
	  && check_range_ie(mipsaddr, size, CODE_BOTTOM, CODE_TOP))

      /* Access unallocated on HEAP */
      || (check_range_ie(mipsaddr, size, HEAP_BOTTOM, HEAP_TOP)
	   && !find_allocated_block_in_heap(mipsaddr, size))

      /* Unaligned adress */
      || (mipsaddr % size)

      /* Above MEM_SIZE */
      || ((mipsaddr + size) >= MEM_SIZE)
      );
}

/* ***************************************************** */
/* Load and store in virtual memory                      */
/* ***************************************************** */

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

/* ***************************************************** */
/* Malloc/Free blocks in virtual memory                  */
/* ***************************************************** */

/* Allocatw a new wrapper for block info */
static block * new_block(bool used, uint32_t mipsaddr, uint32_t size) {
  block * new;
  if (!(new = malloc(sizeof(*new))))
    allocation_failed();
  new->used = used;
  new->mipsaddr = mipsaddr;
  new->size = size;
  return new;
}

/* Two consecutive free blocks should be considered as one block */
static void merge_blocks(void) {
  item * curr, * previous, * next;
  bool flag = true;
  while (flag) { /* stop trying to merge when no merge has been done */
    flag = false;
    curr = heap->begin;
    previous = NULL;
    do {
      next = curr->next;
      if (previous
	  && !((block *)(previous->data))->used
	  && !((block *)(curr->data))->used) {
	((block*)(previous->data))->size += ((block*)(curr->data))->size;
	list_delete_item(heap, curr, free);
	previous = NULL;
	flag = true;
      } else {
	previous = curr;
      }
      curr = next;
    } while (curr != heap->begin);
  }
}

/* Use an existing freed block to allocate a new block
 * Divide it when necessary */
static block * use_this_item(item * this_item, uint32_t size) {
  block * this_block;
  this_block = this_item->data;
  if (!(this_block->size == size))
    list_insert_after(heap, this_item,
		      new_block(false,
				this_block->mipsaddr + size,
				this_block->size - size));
  this_block->used = true;
  this_block->size = size;
  merge_blocks();
  return this_block;
}

/* No block freed available, so create a new block */
static block * allocate_new_block(uint32_t size) {
  block * result;
  heap = list_insert(heap, (result = new_block(true, last_address, size)));
  last_address += size;
  merge_blocks();
  return result;
}

/* Uses the first fit algorithm to allocate a block of size bytes 
 * in the heap. Returns the address of the start of the block 
 * if allocation is successful, returns 0 on failure. */
uint32_t first_fit_malloc(uint32_t size) {
  item * available;
  if (!size)
    return 0;
  while (size % SIZE_WORD) /* always allocate on 4 bytes aligned */
    ++size;
  return
    ((available = find_available_item_in_heap(size)) ?
     use_this_item(available, size)
     : allocate_new_block(size))->mipsaddr;
}

/* Free the allocated block of memory in the heap beginning at
 * mipsaddr. In order to successfully free a block, mipsaddr must 
 * be the beginning of a block that was allocated on the heap. 
 * If block_free() is called on an unallocated memory address or an address
 * that is not the beginning of a block, bad_free() should be called. */
void block_free(uint32_t mipsaddr){
  block * b;
  if (!(b = find_allocated_block_in_heap_any_size(mipsaddr)))
    bad_free(mipsaddr);
  b->used = false;
  merge_blocks();
}

/* ***************************************************** */
/* Extra Credit Super Malloc                             */
/* ***************************************************** */

/** Extra Credit **/
/* Wait until completing the rest of the project to work on this part; it
 * requires significantly more work than first_fit_malloc().
 * You may need to declare new variables and/or modify functions you 
 * have already completed. */
uint32_t next_fit_malloc(uint32_t size){
  /* TODO YOUR CODE HERE */
  return size;
}


