#ifndef _LOAD_PROGRAM_H
#define _LOAD_PROGRAM_H

#include <stdint.h>
#include <string.h>
#include "elf.h"

void load_program(uint8_t* mem, size_t memsize, const char* fn, int disasm);

#endif
