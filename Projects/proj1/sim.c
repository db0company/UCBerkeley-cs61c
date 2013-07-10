#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "load_program.h"
#include "processor.h"
#include "disassemble.h"

int main(int argc, char** argv)
{
  /* options */
  int opt_disasm = 0, opt_regdump = 0, opt_interactive = 0;

  /* the architectural state of the CPU */
  processor_t p;

  /* parse the command-line args */
  int c;
  while ((c = getopt(argc, argv, "drit")) != -1)
  {
    switch (c)
    {
    case 'd':
      opt_disasm = 1;
      break;

    case 'r':
      opt_regdump = 1;
      break;

    case 'i':
      opt_interactive = 1;
      break;

    case 't':
      opt_interactive = 2;
      break;

    default:
      fprintf(stderr, "Bad option %c\n", c);
      return -1;
    }
  }

  /* make sure we got an executable filename on the command line */
  if (argc <= optind)
  {
    fprintf(stderr, "Give me an executable file to run!\n");
    return -1;
  }

  /* load the executable into memory */
  load_program(init_mem(), MEM_SIZE, argv[optind], opt_disasm);

  /* if we're just disassembling, exit here */
  if (opt_disasm)
  {
    return 0;
  }

  /* initialize the CPU */
  init_processor(&p);

  /* simulate forever! */
  while (1)
  {
    execute_one_inst(&p, opt_interactive, opt_regdump);
  }
  
  return 0;
}
