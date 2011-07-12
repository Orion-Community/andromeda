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

#ifndef MAP_H
#define MAP_H

#include <mm/paging.h>
#include <types.h>
#include <boot/mboot.h>

#define FREE	   0x0000
#define MODULE	   0x0001
#define COMPRESSED 0x0002
#define MAPPEDIO   0x0003
#define NOTUSABLE  0xFFFF

#define MAX_MODS   0x20


typedef struct
{
  unsigned long addr;
  boolean usable;
} pageState_t;

struct module_s
{
  unsigned long addr;
  unsigned long end;
};

typedef struct module_s module_t;

extern module_t modules[];
extern unsigned short bitmap[];

boolean claimPage(unsigned long page, unsigned short owner);
pageState_t* allocPage(unsigned short owner);
void freePage(void* page, unsigned short owner);

void buildMap(multiboot_memory_map_t*, int);

#endif