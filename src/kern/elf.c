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
#include <mm/map.h>

// Just a magic number
#define ELFMAGIC 0x7F454C46

//Needed for the core image

// Used to check the validity of the Elf header
boolean elfCheck(Elf32_Ehdr* hdr)
{
  unsigned char* e_ident = hdr->e_ident;
  int i;
  #ifdef MODS
  printf("Addr: 0x%x\n", (int)hdr);
  printf("ELF magic: 0x%x%c%c%c\n", (unsigned int)e_ident[EI_MAG0], e_ident[EI_MAG1], e_ident[EI_MAG2], e_ident[EI_MAG3]);
  #endif
  if (*((unsigned int*)e_ident)==ELFMAGIC)
  {
    return FALSE;
  }
  // Some things that have to be assesed individually. Hate this way of writing things but unfortunately
  // the only way I can think of as of now.
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

int coreAugment(void* image)
{
  Elf32_Ehdr *elfHeader = (Elf32_Ehdr*) image;
  Elf32_Off address = elfHeader->e_phoff;
  if (address == 0)
  {
    return 0;
  }
  void *programHeader = (void*)(((unsigned long)elfHeader) + ((unsigned long)address));
  void *thisHeader = NULL;
  int noHdrs = elfHeader->e_phnum;
  int hdrSize = elfHeader->e_phentsize;
  int i = 0;
  for (thisHeader = programHeader; i < noHdrs; i++)
  {
    Elf32_Phdr* hdr = (Elf32_Phdr*)thisHeader;
    #ifdef ELFDBG
    printf("Header: %X\tType: %X\tMemsz: %X\tFilesz: %X\n", (int)hdr, hdr->p_type, hdr->p_memsz, hdr->p_filesz);
    #endif
    if (hdr->p_type != 0x6474E551)
      
    if (hdr->p_memsz == 0) continue;
    memcpy((void*)hdr->p_vaddr, (void*)(hdr->p_offset+(unsigned int) image), hdr->p_filesz);
    if (hdr -> p_memsz > hdr -> p_filesz)
      memset((void*)(hdr->p_vaddr + hdr->p_filesz), 0, hdr->p_memsz - hdr->p_filesz);
    
    // Memory protection flags should probably be set here.
    
    thisHeader+=hdrSize;
  }
  return 0;
}

int coreCheck(void* image)
{
  if (!elfCheck(image))
    return -1;
  Elf32_Ehdr *elfHeader = (Elf32_Ehdr*) image;
  if (elfHeader->e_entry < 0xC0000000)
    return -2;
  
  Elf32_Off address = elfHeader->e_phoff;
  if (address == 0)
  {
    return 0;
  }
  void *programHeader = (void*)(((unsigned long)elfHeader) + ((unsigned long)address));
  void *thisHeader = NULL;
  int noHdrs = elfHeader->e_phnum;
  int hdrSize = elfHeader->e_phentsize;
  int i = 0;
  
  for (thisHeader = programHeader; i < noHdrs; i++)
  {
    Elf32_Phdr* hdr = (Elf32_Phdr*)thisHeader;
    if (*(int*)(hdr->p_offset+(unsigned int) image) == 0xC0DEBABE)
      return 0;
    thisHeader+=hdrSize;
  }
  return 3;
}

void elfJmp(void* image)
{
  Elf32_Ehdr *elfHeader = (Elf32_Ehdr*) image;
  void* addr = (void*)(elfHeader->e_entry);
  #ifdef ELFDBG
  printf("entry point: 0x%X\n", addr);
  #endif
//   elfJump(addr, bitmap, modules);
}

/*
 * Run the elf image passed on to this function. Shouldn't return at all
 * but just in case it does, the return types are:
 * 0 for failure
 * 1 for success
 */
int elfExec(void* image)
{
  if (elfCheck(image))
  {
    Elf32_Ehdr *elfHeader = (Elf32_Ehdr*) image;
    Elf32_Off address = elfHeader->e_phoff;
    if (address == 0)
    {
      return 0;
    }
    void *programHeader = (void*)(((unsigned long)elfHeader) + ((unsigned long)address));
    void *thisHeader = NULL;
    int noHdrs = elfHeader->e_phnum;
    int hdrSize = elfHeader->e_phentsize;
    int i = 0;
    printf("Program header\n");
    for (thisHeader = programHeader; i < noHdrs; i++)
    {
      Elf32_Phdr* hdr = (Elf32_Phdr*)thisHeader;
      printf("Type:\t0x%X\tOffset:\t0x%X\nvaddr:\t0x%X\tSize:\t0x%X\nAlign:\t0x%X\tFlags:\t0x%X\n\n",
						hdr->p_type, hdr->p_offset, hdr->p_vaddr, 
						hdr->p_memsz, hdr->p_align, hdr->p_flags);
      thisHeader+=hdrSize;
    }
    #ifdef ELFDBG
    int j;
    for (j = 0; j < 0x1FFFFFFF; j++);
    #endif
    Elf32_Off sectionAddress = elfHeader->e_shoff;
    if (address != 0 && FALSE)
    {
      void *sectionHeader = (void*)(((unsigned long)elfHeader) + ((unsigned long)sectionAddress));
      void *thisSection = NULL;
      int noSHdrs = elfHeader->e_shnum;
      int shdrSize = elfHeader->e_shentsize;
      i = 0;
      printf("Section header\n");
      for (thisSection = sectionHeader; i < noHdrs; i++)
      {
	Elf32_Shdr* hdr = (Elf32_Shdr*)thisSection;
	printf("Type:\t0x%X\tOffset:\t0x%X\nAddr:\t0x%X\tSize:\t0x%X\nAlign:\t0x%X\tFlags:\t0x%X\nName:\t0x%X\n\n",
						hdr->sh_type, hdr->sh_offset, hdr->sh_addr,
						hdr->sh_size, hdr->sh_addralign, hdr->sh_flags,
						hdr->sh_name);
	#ifdef ELFDBG
	for (j = 0; j < 0xfffffff; j++);
	#endif
      }
      
    }
  }
  return 1;
}