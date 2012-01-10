/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __KERN_ELF_H
#define __KERN_ELF_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#define SHF_WRITE	0x1
#define SHF_ALLOC	0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASKPROC	0xf0000000

typedef struct
{
	Elf32_Word 	sh_name;
	Elf32_Word 	sh_type;
	Elf32_Word 	sh_flags;
	Elf32_Addr 	sh_addr;
	Elf32_Off  	sh_offset;
	Elf32_Word 	sh_size;
	Elf32_Word 	sh_link;
	Elf32_Word 	sh_info;
	Elf32_Word 	sh_addralign;
	Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct
{
	Elf32_Word	st_name;
	Elf32_Addr	st_value;
	Elf32_Word	st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half	st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i) ((i)>>4)
#define ELF32_ST_TYPE(i) ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4+(t)&0xf))

#define STB_LOCAL	0x0
#define STB_GLOBAL	0x1
#define STB_WEAK	0x2
#define STB_LOPROC	0xD
#define STB_HIPROC	0xF

#define STT_NOTYPE	0x0
#define STT_OBJECT	0x1
#define STT_FUNC	0x2
#define STT_SECTION	0x3
#define STT_FILE	0x4
#define STT_LOPROC	0xD
#define STT_HIPROC	0xF

typedef struct
{
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
} Elf32_Rel;

typedef struct
{
	Elf32_Addr	r_offset;
	Elf32_Word	r_info;
	Elf32_Sword	r_addend;
} Elf32_Rela;

#define ELF32_R_SYM (i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s,t) ((s)<<8+(unsigned char)(t))

#define R_386_NONE	0x0
#define R_386_32	0x1
#define R_386_PC32	0x2
#define R_386_GOT32	0x3
#define R_386_PLT32	0x4
#define R_386_COPY	0x5
#define R_386_GLOB_DAT	0x6
#define R_386_JMP_SLOT	0x7
#define R_386_RELATIVE	0x8
#define R_386_GOTOFF	0x9
#define R_386_GOTPC	0xA

typedef struct
{
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

#define PT_NULL		0x0
#define PT_LOAD		0x1
#define PT_DYNAMIC	0x2
#define PT_INTERP	0x3
#define PT_NOTE		0x4
#define PT_SHLIB	0x5
#define PT_PHDR		0x6
#define PT_LOOS		0x60000000	/* Start of OS-specific */
#define PT_GNU_EH_FRAME	0x6474e550	/* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK	0x6474e551	/* Indicates stack executability */
#define PT_GNU_RELRO	0x6474e552	/* Read-only after relocation */
#define PT_LOSUNW	0x6ffffffa
#define PT_SUNWBSS	0x6ffffffa	/* Sun Specific segment */
#define PT_SUNWSTACK	0x6ffffffb	/* Stack segment */
#define PT_HISUNW	0x6fffffff
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff

#define DT_NULL		0x0
#define DT_NEEDED	0x1
#define DT_PLTRELSZ	0x2
#define DT_PLTGOT	0x3
#define DT_HASH		0x4
#define DT_STRTAB	0x5
#define DT_SYMTAB	0x6
#define DT_RELA		0x7
#define DT_RELASZ	0x8
#define DT_RELAENT	0x9
#define DT_STRSZ	0xA
#define DT_SYMENT	0xB
#define DT_INIT		0xC
#define DT_FINI		0xD
#define DT_SONAME	0xE
#define DT_RPATH	0xF
#define DT_SYMBOLIC	0x10
#define DT_REL		0x11
#define DT_RELSZ	0x12
#define DT_RELENT	0x13
#define DT_PLTREL	0x14
#define DT_DEBUG	0x15
#define DT_TEXTREL	0x16
#define DT_JMPREL	0x17
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

typedef struct
{
	Elf32_Sword
	d_tag;
	union {
		Elf32_Word d_val;
		Elf32_Addr d_ptr;
	} d_un;
} Elf32_Dyn;


boolean checkHdr(Elf32_Ehdr* hdr);
int elfExec(void* image);
int coreCheck(void* image);

extern void elfJump(void* addr, void* memoryMap, void* modules);

#ifdef __cplusplus
}
#endif

#endif