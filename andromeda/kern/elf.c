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
  return TRUE;
}