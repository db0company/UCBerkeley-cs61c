// This file contains the code to load an executable into memory.
// You shouldn't have to edit this file, or understand it, for that matter.

#include "load_program.h"
#include "disassemble.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// die loudly if cond is 0
static void demand(int cond, const char* str, ...)
{
  if(cond)
    return;

  va_list vl;
  va_start(vl,str);

  vfprintf(stderr, str, vl);
  fputs("\n",stderr);
  exit(-1);

  va_end(vl);
}

// load program "fn" into memory
void load_program(uint8_t* mem, size_t memsize, const char* fn, int disasm)
{
  int fd, i, j;
  size_t size;
  struct stat s;
  char* buf;
  const Elf32_Ehdr* eh;
  const Elf32_Shdr* sh;
  const Elf32_Phdr* ph;

  fd = open(fn, O_RDONLY);
  demand(fd != -1, "Couldn't open executable file %s!", fn);

  demand(fstat(fd,&s) != -1, "Couldn't access executable file %s!", fn);
  size = s.st_size;

  buf = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  eh = (const Elf32_Ehdr*)buf;
  demand(buf != MAP_FAILED, "Couldn't read executable file %s!", fn);

  close(fd);

  demand(size >= sizeof(Elf32_Ehdr) &&
         strncmp((const char*)eh->e_ident,ELFMAG,strlen(ELFMAG)) == 0 &&
         eh->e_ident[EI_CLASS] == ELFCLASS32 &&
         size >= eh->e_phoff + eh->e_phnum*sizeof(Elf32_Ehdr),
         "Invalid executable file %s!", fn);

  sh = (const Elf32_Shdr*)(buf+eh->e_shoff);
  for(i = 0; i < eh->e_shnum; i++, sh++)
  {
    if(disasm)
    {
      if(sh->sh_type == SHT_PROGBITS && (sh->sh_flags & SHF_EXECINSTR))
      {
        for(j = 0; j < sh->sh_size/4; j++)
        {
          printf("%08x: ", sh->sh_offset + 4*j);
          disassemble(*(inst_t*)((uint8_t*)buf + sh->sh_offset + 4*j));
        }
      }
    }
  }

  ph = (const Elf32_Phdr*)(buf+eh->e_phoff);
  for(i = 0; i < eh->e_phnum; i++, ph++)
  {
      if(ph->p_type == SHT_PROGBITS && ph->p_memsz)
      {
        demand(size >= ph->p_offset + ph->p_filesz &&
               ph->p_vaddr + ph->p_memsz <= memsize,
               "Invalid executable file %s!", fn);
        memcpy(mem + ph->p_vaddr, (uint8_t*)buf + ph->p_offset, ph->p_filesz);
        memset(mem + ph->p_vaddr+ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
      }
  }
}
