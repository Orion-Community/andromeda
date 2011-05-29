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

/*
 * Comments disclaimer:
 * If you want to know what all these variables mean.
 * They have been specified in the ELF32 specification.
 *
 * http://www.skyfree.org/linux/references/ELF_Format.pdf
 */

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

#define ELFCLASSNONE	0x0
#define ELFCLASS32	0x1
#define ELFCLASS64	0x2

#define ELFDATANONE	0x0
#define ELFDATA2LSB	0x1
#define ELFDATA2MSB	0x2

#define ELFMAG0		0x7f
#define ELFMAG1		'E'
#define ELFMAG2		'L'
#define ELFMAG3		'F'

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

#define SHN_UNDEF	0x0
#define SHN_LORESERVE	0xff00
#define SHNLOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN HIRESERVE	0xffff

#define SHT_NULL	0x0
#define SHT_PRGBITS	0x1
#define SHT_SYMTAB	0x2
#define SHT_STRTAB	0x3
#define SHT_RELA	0x4
#define SHT_HASH	0x5
#define SHT_DYNAMIC	0x6
#define SHT_NOTE	0x7
#define SHT_NOBITS	0x8
#define SHT_REL 	0x9
#define SHT_SHLIB	0xA
#define SHT_DYNSYM	0xB
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0x8fffffff

typedef struct
{
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off  sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

boolean checkHdr(Elf32_Ehdr* hdr);
int elfExec(void* image);

#endif