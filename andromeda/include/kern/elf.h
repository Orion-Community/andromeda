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

#ifndef ELF_H
#define ELF_H

#include <stdlib.h>

typedef unsigned int    Elf32_Addr;
typedef unsigned short  Elf32_Half;
typedef unsigned int    Elf32_Off;
typedef signed int      Elf32_Sword;
typedef unsigned int    Elf32_Word;

#define ET_NONE		0x0
#define ET_REL		0x1
#define ET_EXEC		0x2
#define ET_DYN		0x3
#define ET_CORE		0x4
#define ET_LOPROC	0xFF00
#define ET_HIPROC	0xFFFF

#define EM_NONE		0x0
#define EM_M32		0x1
#define EM_SPARC	0x2
#define EM_386		0x3
#define EM_68K		0x4
#define EM_88K		0x5
#define EM_860		0x6
#define EM_MIPS		0x7

#define EV_NONE		0x0
#define EV_CURRENT	0x1

#define EI_MAG0		0x0
#define EI_MAG1		0x1
#define EI_MAG2		0x2
#define EI_MAG3		0x3
#define EI_CLASS	0x4
#define EI_DATA		0x5
#define EI_VERSION	0x6
#define EI_PAD		0x7
#define EI_NIDENT 	0x10

typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

#endif