/* 
 * CS61C Summer 2013
 * Name: Barbara Lepage
 * Login: gm
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PADDING		2
#define KEY_SIZE	(PADDING * 2) + 1

typedef unsigned int	uint;
typedef char *		row_t;
typedef uint		rule_t;
typedef char		kkey_t;

bool usage(void);
bool life1d(uint, rule_t);
bool init(size_t, row_t *, row_t *);
void print_details(uint, rule_t);
void print_row(row_t, size_t);
void apply_rule(row_t, row_t, size_t, rule_t);
kkey_t generate_key(row_t);
bool dead_or_alive(kkey_t, rule_t);
void exchange(row_t *, row_t *);
void destroy(row_t, row_t);
bool check_args(int, char **, uint *, rule_t *);

int	main(int argc, char ** argv) {
  uint rows;
  rule_t rule;
  return check_args(argc, argv, &rows, &rule)
    && life1d(rows, rule) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* life1d is an elementary cellular automaton.
 * rows is the number of successive generations (after the initial)
 * rule is a 32-bit integer specifying the rule to use when computing aliveness/deadness */
bool	life1d(uint rows, rule_t rule) {
  row_t current_row, next_row;
  size_t size = 2 * rows + 1 + (2 * PADDING);
  size_t i;

  if (!init(size, &current_row, &next_row))
    return false;

  print_details(rows, rule);
  print_row(current_row, size);

  for (i = 0; i < rows; ++i) {
    apply_rule(current_row, next_row, size, rule);
    exchange(&current_row, &next_row);
    print_row(current_row, size);
  }

  destroy(current_row, next_row);
  return true;
}

/* Initializes two segments of memory of 0s, each with enough memory for padding too
 * + put the first alive cell in the middle of the row */
bool	init(size_t size, row_t * current_row, row_t * next_row) {
  if (!(*current_row = (char *)calloc(size, sizeof(**current_row)))
      || !(*next_row = (char *)calloc(size, sizeof(**next_row)))) {
    fprintf(stderr, "Out of memory.\n");
    return false;
  }
  (*current_row)[size / 2] = 1;
  return true;
}

/* Print details of this run of Life1D */
void	print_details(uint rows, rule_t rule) {
  printf("P1 %u %u ## %u rows of Life1D (Rule %u)\n", 2 * rows + 1, rows + 1, rows, rule);
}

/* Print a row */
void	print_row(row_t row, size_t size) {
  size_t i;
  for (i = PADDING; i < size - PADDING; ++i) {
    printf("%d", row[i]);
    if (i < size - PADDING - 1)
      printf(" ");
  }
  printf("\n");
}

/* Apply the rule to the row by changing the content of the cells */
void	apply_rule(row_t current_row, row_t next_row, size_t size, rule_t rule) {
  size_t i;
  for (i = PADDING; i < size - PADDING; ++i)
    next_row[i] = dead_or_alive(generate_key(&current_row[i]),
				rule) ? 1 : 0;
}

/* Take a pointer to a cell, create a key using the cells before and the cells after */
kkey_t	generate_key(row_t cell) {
  kkey_t key = 0;
  size_t i;
  cell -= PADDING;
  for (i = 0; i < KEY_SIZE; ++i, ++cell)
    if (*cell)
      key |= 1 << (KEY_SIZE - i - 1);
  return key;
}

/* Take a key, check in the rule if it's alive and return true, false if dead */
bool	dead_or_alive(kkey_t key, rule_t rule) {
  return (rule >> key) & 1;
}

/* This function exchange the two rows pointers because the next row is now
 * the current row, so we will use the old current row to generate the next
 * raw. We don't need to re-initialize the next raw since we will erase
 * all the values in the apply_rule function */
void	exchange(row_t * current_row, row_t * next_row) {
  row_t tmp = *current_row;
  *current_row = *next_row;
  *next_row = tmp;
}

/* Frees initialized memory */
void	destroy(row_t current_row, row_t next_row) {
  free(current_row);
  free(next_row);
}

/* Perform input checking on arguments. First checks for correct number of arguments, and
 * then uses the strtoul() function to convert each argument into an uint. Also
 * checks whether each argument is an integer or not. If any of these requirements are 
 * not met, print the usage string instead.
 */
bool	check_args(int argc, char ** argv, uint * rows, rule_t * rule) {
  char ** endptr;
  if (argc != 3)
    return usage();
  *rows = (uint)strtoul(argv[1], (endptr = argv), 10);
  if(**endptr)
    return usage();
  *rule = (rule_t)strtoul(argv[2], (endptr = argv), 10);
  if(**endptr)
    return usage();
  return true;
}

/*
 * Prints the usage text if inputs do not satisfy the valid constraints. Do not modify
 * this function. 
 */
bool usage(void) {
  printf("Usage: Life1D <rows> <rule>\n");
  printf("    This program simulates 1D Life: the simplest class of one-dimensional\n");
  printf("    cellular automata in a <ROWS=rows+1> x <COLS=2*rows+1> grid starting\n");
  printf("    with a single live cell in the middle of the top row using rule <rule>.\n");
  printf("    These 1D rules are defined in Wolfram's Elementary Cellular Automata:\n");
  printf("    http://mathworld.wolfram.com/ElementaryCellularAutomaton.html\n");
  printf("    This program will print to stdout data in plain PBM file format.\n");
  printf("    This output can be easily viewed using the display command or \n");
  printf("    converted to a another format using the pbmto* and ppmto* utilities. \n");
  printf("    A plain ASCII PBM file can be created by adding a header line \n");
  printf("    \"P1 <WIDTH> <HEIGHT>\" and followed by a grid of data \n");
  printf("    (0 = dead = white, 1 = live = black). Add a comment on the first \n");
  printf("    line with a brief description of the image.\n");
  printf("  Arguments:\n");
  printf("    <rows> is a positive integer specifying the number of rows to generate\n");
  printf("    (not counting the first \"seed row\" which is all dead except for a central\n");
  printf("    live cell). The columns are computed automatically -- enough so that\n");
  printf("    the rule, if it were to grow in the normal triangular pattern, would\n");
  printf("    just perfectly reach the edge. Off the board is considerered \"dead\".\n");
  printf("    <rule> is a number from 0-4294967295 specifying the rule to use.\n");

  return false;
}
