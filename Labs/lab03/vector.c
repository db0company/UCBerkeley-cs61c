/* Include the system headers we need */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/* Include our header */
#include "vector.h"

/* Define what our struct is */
struct vector_t {
	size_t size;
	int *data;
};

/* Utility function to handle allocation failures. In this
   case we print a message and exit. */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(1);
}

static void null_pointer_error() {
  fprintf(stderr, "vector_get: passed a NULL vector.\n");
  abort();
}

/* Create a new vector */
vector_t *vector_new() {
	vector_t *retval;  

	/* First, we need to allocate the memory for the struct */
	retval = malloc(1 * sizeof(vector_t));

	/* Check our return value to make sure we got memory */
	if(retval == NULL)
                allocation_failed();
	 
	/* Now we need to initialize our data */
	retval->size = 1;
	retval->data = malloc(retval->size * sizeof(int));

	/* Check our return value to make sure we got memory */
	if(retval->data == NULL) {
		free(retval);
                allocation_failed();
	}

	retval->data[0] = 0;
	
	/* and return... */
	return retval;
}

/* Free up the memory allocated for the passed vector */
void vector_delete(vector_t *v) {
	/* Remember, you need to free up ALL the memory that is allocated */
  if (!v)
    return ;
  free(v->data);
  free(v);
}

/* Return the value in the vector */
int vector_get(vector_t *v, size_t loc) {

	/* If we are passed a NULL pointer for our vector, complain about it and
         * exit.
	 */
	if(v == NULL)
	  null_pointer_error();

	/* If the requested location is higher than we have allocated, return 0.
	 * Otherwise, return what is in the passed location.
	 */
	if(loc < v->size) {
		return v->data[loc];
	} else {
		return 0;
	}
}

/* Display the content of the vector */
void vector_dump(int * v, ssize_t s) {
  size_t i;
  for (i = 0; i < s && i < 30; ++i)
    printf("%d ", v[i]);
  printf("%s\n", i == s ? "" : "...");
}

/* Set a value in the vector. If the extra memory allocation fails, call
   allocation_failed(). */
void vector_set(vector_t *v, size_t loc, int value) {
	/* What do you need to do if the location is greater than the size we have
	 * allocated?  Remember that unset locations should contain a value of 0.
	 */
  int * new;
  ssize_t i;
  if (!v)
    null_pointer_error();
  if (loc >= v->size) {
    if (!(new = malloc((loc + 1) * sizeof(int))))
      allocation_failed();
    bzero(new, (loc + 1) * sizeof(int));
    for (i = 0; i < v->size; ++i)
      new[i] = v->data[i];
    free(v->data);
    v->data = new;
    v->size = loc + 1;
  }
  v->data[loc] = value;
}
