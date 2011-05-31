/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <kern/elf.h>

boolean checkHdr(Elf32_Ehdr* hdr)
{
  unsigned char* e_ident = hdr->e_ident;
  int i;
  #ifdef MODS
  printf("Addr: 0x%x\n", (int)hdr);
  printf("ELF magic: 0x%x%c%c%c\n", (unsigned int)e_ident[EI_MAG0], e_ident[EI_MAG1], e_ident[EI_MAG2], e_ident[EI_MAG3]);
  #endif
  if (e_ident[EI_MAG0]!= ELFMAG0 || e_ident[EI_MAG1] != ELFMAG1 || e_ident[EI_MAG2] != ELFMAG2 || e_ident[EI_MAG3] != ELFMAG3)
  {
    return FALSE;
  }
  if (e_ident[EI_CLASS]!=ELFCLASS32)
  {
    #ifdef MODS
    printf("ELF class: 0x%x\n", e_ident[EI_CLASS]);
    #endif
    return FALSE;
  }
  if (e_ident[EI_DATA] != ELFDATA2LSB)
  {
    #ifdef MODS
    printf("ELF data: 0x%x\n", e_ident[EI_DATA]);
    #endif
    return FALSE;
  }
  if (hdr->e_machine != EM_386)
  {
    #ifdef MODS
    printf("Machine type: 0x%x\n", hdr->e_machine);
    #endif
  }
  #ifdef WARN
  if (hdr->e_flags != 0x0)
  {
    printf("WARNING! The elf flags aren't 0\n");
  }
  #endif
  return TRUE;
}

Elf32_Phdr* getPhdr(Elf32_Ehdr *elfHeader)
{
  Elf32_Off address = elfHeader->e_phoff;
  if (address == 0)
  {
    return NULL;
  }
  Elf32_Phdr *programHeader = (Elf32_Phdr*)(((int)elfHeader) + ((int)address));
  return programHeader;
}

int elfExec(void* image)
{
  if (checkHdr(image))
  {
    Elf32_Phdr *header = getPhdr(image);
    if (header == NULL)
    {
      return 1;
    }
    return 0;
  }
  return 1;
}