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


/*
 * This is the inital boot image loaded from
 * grub. Thi
	initHeap(K128, K128);s will call the actual kernel
 * from where-ever it might be. From disk
 * (hope-not) of from gzip image in memory
 * (would be alot better).
 */

typedef struct test
{
      int a, b, c, d, e, f, g, h, i, j, k, l, m;
} test_t;

#include <text.h>
#include <types.h>
#include <mm/memory.h>
#include <mm/heap.h>

#define K128 0x8000000

memNode_t* getHdr(void* ptr)
{
	return (memNode_t*) ptr-sizeof(memNode_t);
}

void announce()
{
	textInit();
	println("Compressed kernel loaded");
	println("Decompressing the kernel");
}

int kmain(/* boot data , boot data , gzipped kernel*/)
{
	announce();
	//setGDT();
	initHeap(K128, K128);
	
	test_t* a = alloc(sizeof(test_t), FALSE);
	test_t* b = alloc(sizeof(test_t), FALSE);
	
	printf("a\t\tb\t\tsize\n");
	printhex((int)a); putc('\t');
	printhex((int)b); putc('\t');
	printhex(sizeof(test_t)); putc('\n');
	
	printf("\na.size\t\tb.size\t\tsize\n");
	
	printhex((int)(getHdr(a)->size)); putc('\t');
	printhex((int)(getHdr(b)->size)); putc('\t');
	printhex(sizeof(test_t)); putc('\n');
	
	//installInterruptVectorTable();
	//initPaging();
	//exec(decompress(gzipped kernel));

	for (;;); // Prevent the CPU from
		  // doing stupid things.
	return 0;
}
