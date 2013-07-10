/* 
 * CS61C Summer 2013
 * Name: Barbara Lepage
 * Login: gm
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flights.h"
#include "timeHM.h"

/* ***************************************************** */
/*                     GLOBAL STUFF                      */
/* ***************************************************** */

/*
   This should be called if memory allocation failed.
 */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}

char * strdup(const char *str) {
  char * dup;
  if (!(dup = malloc((strlen(str) + 1) * sizeof(*dup))))
    allocation_failed();
  strcpy(dup, str);
  return dup;
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

/* Insert a new element to a list.
 * Create a new list if the list pointer is NULL */

list *	list_insert(list * l, void * data) {
  item * new;
  if (!(new = malloc(sizeof(*new))))
    allocation_failed();
  new->data = data;
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
 * return true.
 * A param is given to the function. If you don't want to use it, give NULL.
*/
void *	list_find(list * list, bool (* f)(void *, void *), void * param) {
  item * curr;
  size_t i;
  if (!list)
    return NULL;
  curr = list->begin;
  for (i = 0; i < list->size; ++i, curr = curr->next)
    if (f(curr->data, param))
      return curr->data;
  return NULL;
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
/*                      FLIGHTS STUFF                    */
/* ***************************************************** */

struct airport {
  char * name;
  list * schedules;
};

typedef struct {
  char * airport_name;
  timeHM_t departure;
  timeHM_t arrival;
  int cost;
} schedule;

struct flightSys {
  list * airports;
};

/* ***************************************************** */
/*                       CREATE/ADD                      */
/* ***************************************************** */

/*
   Creates and initializes a flight system, which stores the flight schedules of several airports.
   Returns a pointer to the system created.
 */
flightSys_t* createSystem(void) {
  flightSys_t * s;
  if (!(s = malloc(sizeof(*s))))
    allocation_failed();
  s->airports = NULL;
  return s;
}

/*
   Adds a airport with the given name to the system. You must copy the string and store it.
   Do not store "name" (the pointer) as the contents it point to may change.
 */
void addAirport(flightSys_t* s, char* name) {
  airport_t * a;
  if (!s)
    return ;
  if (!(a = malloc(sizeof(*a))))
    allocation_failed();
  a->name = strdup(name);
  a->schedules = NULL;
  s->airports = list_insert(s->airports, a);
}

/*
   Adds a flight to src's schedule, stating a flight will leave to dst at departure time and arrive at arrival time.
 */
void addFlight(airport_t* src, airport_t* dst, timeHM_t* departure, timeHM_t* arrival, int cost) {
  schedule * s;
  if (!src || !dst || !departure || !arrival)
    return ;
  if (!(s = malloc(sizeof(*s))))
    allocation_failed();
  s->airport_name = strdup(dst->name);
  s->departure = *departure; /* memcpy(&(s->departure), departure, sizeof(*departure)); */
  s->arrival = *arrival; /* memcpy(&(s->arrival), arrival, sizeof(*arrival)); */
  s->cost = cost;
  src->schedules = list_insert(src->schedules, s);
}

/* ***************************************************** */
/*                         DELETE                        */
/* ***************************************************** */

void	deleteSchedule(void * vs) {
  schedule * s = (schedule *)vs;
  if (!s)
    return ;
  free(s->airport_name);
  free(s);
}

void	deleteAirport(void * va) {
  airport_t * a = (airport_t *)va;
  if (!a)
    return ;
  free(a->name);
  list_delete(a->schedules, deleteSchedule);
  free(a);
}

/*
   Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteSystem(flightSys_t* s) {
  if (!s)
    return ;
  list_delete(s->airports, deleteAirport);
  free(s);
}

/* ***************************************************** */
/*                        GET                            */
/* ***************************************************** */

bool	findString(void * va, void * name) {
  airport_t * a = (airport_t *)va;
  return !strcmp(a->name, (char *)name);
}

/*
   Returns a pointer to the airport with the given name.
   If the airport doesn't exist, return NULL.
 */
airport_t* getAirport(flightSys_t* s, char* name) {
  if (!s)
    return NULL;
  return list_find(s->airports, findString, name);
}

/*
   Given a src and dst airport, and the time now, finds the next flight to take based on the following rules:
   1) Finds the cheapest flight from src to dst that departs after now.
   2) If there are multiple cheapest flights, take the one that arrives the earliest.

   If a flight is found, you should store the flight's departure time, arrival time, and cost in departure, arrival, 
   and cost params and return true. Otherwise, return false. 

   Please use the function isAfter() from time.h when comparing two timeHM_t objects.
 */

typedef struct {
  char * dst;
  timeHM_t * now;
  schedule * best;
} fpar;

void	findNextFlight(void * vs, void * vp) {
  schedule * s = (schedule *)vs;
  fpar * p = vp;
  if (!s || !p)
    return ;
  if (strcmp(s->airport_name, p->dst) /* not the right airport */
      || isAfter(p->now, &(s->departure)))  /* too late */
    return ;
  if (!p->best
      || s->cost < p->best->cost /* better price */
      || (p->best->cost == s->cost /* same price but arrive before */
	  && isAfter(&(p->best->arrival), &(s->arrival))))
    p->best = s;
}

bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival, int* cost) {
  fpar p;
  if (!src || !dst)
    return false;
  p.dst = dst->name;
  p.now = now;
  p.best = NULL;
  list_foreach(src->schedules, findNextFlight, &p);
  if (!p.best)
    return false;
  *departure = (p.best)->departure;
  *arrival = (p.best)->arrival;
  *cost = (p.best)->cost;
  return true;
}

/* ***************************************************** */
/*                    PRETTY PRINTER                     */
/* ***************************************************** */

void	printAirportName(void * va, void * _) {
  airport_t * a = (airport_t *)va;
  if (!a)
    return ;
  puts(a->name);
}

/*
   Print each airport name in the order they were added through addAirport, one on each line.
   Make sure to end with a new line. You should compare your output with the correct output
   in flights.out to make sure your formatting is correct.
 */
void printAirports(flightSys_t* s) {
  if (!s)
    return ;
  list_print(s->airports, printAirportName, NULL);
}

void printASchedule(void * vs, void * _) {
  schedule * s = (schedule *)vs;
  if (!s)
    return ;
  printf("%s ", s->airport_name);
  printTime(&(s->departure));
  printf(" ");
  printTime(&(s->arrival));
  printf(" $%d\n", s->cost);
}

void	printAirport(void * va, void * _) {
  airport_t * a = (airport_t *)va;
  if (!a)
    return ;
  list_print(a->schedules, printASchedule, NULL);
}

/*
   Prints the schedule of flights of the given airport.

   Prints the airport name on the first line, then prints a schedule entry on each 
   line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".

   You should use printTime (look in timeHM.h) to print times, and the order should be the same as 
   the order they were added in through addFlight. Make sure to end with a new line.
   You should compare your output with the correct output in flights.out to make sure your formatting is correct.
 */
void printSchedule(airport_t* s) {
  if (!s)
    return ;
  printf("%s\n", s->name);
  list_print(s->schedules, printASchedule, NULL);
}

