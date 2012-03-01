/*
    Andromeda
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

#include <stdlib.h>

#ifdef DBG
#include <boot/mboot.h>

multiboot_memory_map_t* mmap;
size_t mmap_size;
extern unsigned int end;

void testMMap(multiboot_info_t* hdr)
{
	if (hdr->flags && MULTIBOOT_INFO_MEM_MAP)
	{
	printf ("mmap_addr = 0x%x\n", (unsigned int)hdr->mmap_addr);
	printf(", mmap_length = 0x%x\n", (unsigned int)hdr->mmap_length);
	for (mmap = (multiboot_memory_map_t *) hdr->mmap_addr;
		(unsigned long) mmap < hdr->mmap_addr + hdr->mmap_length;
		mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                      + mmap->size + sizeof (mmap->size)))
	{
		printf (" size = 0x%x, base_addr = 0x%x%x length"
						      "= 0x%x%x, type = 0x%x\n",
								(int)mmap->size,
								 mmap->addr>>32,
							mmap->addr & 0xFFFFFFFF,
								mmap->len >> 32,
							   mmap->len&0xFFFFFFFF,
								    mmap->type);
	}
	printf("End pointer = %x\n",(int)&end);
	}

	if (hdr->flags && MULTIBOOT_INFO_MODS)
	{
		int mods = hdr->mods_count;
		multiboot_module_t* mbootModules = (multiboot_module_t*)hdr->mods_addr;
		printf("No. modules: %x\n",mods);
		int i = 0;
		for (; i < mods; i++)
		{
			printf("Base addr = %x\n",mbootModules[i].mod_start);
			printf("End  addr = %x\n",mbootModules[i].mod_end);
			printf("CMD  line = %s\n",(char*)mbootModules[i].cmdline);
		}
	}
	else
	{
		printf("No modules found!\n");
	}
}
#endif

#ifdef MMTEST
#include <stdlib.h>

struct small
{
	int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
};
struct large
{
	int a[1024];
};

typedef struct small small_t;
typedef struct large large_t;

void wait()
{
	int i;
	for (i = 0; i < 0x1FFFFFFF; i++)
	{
	}
}

void testAlloc()
{
	printf("Heap test suite!\n\n");
	large_t* a = alloc(sizeof(large_t), FALSE);
	printf("Addr of A: %x\n",(int)a);
	large_t* b = alloc(sizeof(large_t), TRUE);
	printf("Addr of B: %x\n",(int)a);

	small_t* c = alloc(sizeof(small_t), FALSE);
	printf("Addr of C: %x\n",(int)a);
	small_t* d = alloc(sizeof(small_t), TRUE);
	printf("Addr of D: %x\n",(int)a);

	examineHeap();
	wait();

	free(a);
	free(c);

	examineHeap();
	wait();

	a = alloc(sizeof(large_t), TRUE);
	printf("Addr of A: %x\n",(int)a);
	c = alloc(sizeof(small_t), TRUE);
	printf("Addr of C: %x\n",(int)a);

	examineHeap();
	wait();

	free(a);
	free(b);
	free(c);
	free(d);

	examineHeap();
	wait();
}

#endif

/** \file */

