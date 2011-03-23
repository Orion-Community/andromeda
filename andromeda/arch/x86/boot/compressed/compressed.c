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
 * grub. This will call the actual kernel
 * from where-ever it might be. From disk
 * (hope-not) of from gzip image in memory
 * (would be alot better).
 */

#ifdef TESTALLOC
#define TESTA
#endif
#ifdef TESTALLIGNED
#define TESTA
#endif

#ifdef TESTALLOC
typedef struct
{
	int a,b,c,d,e,f,g,h,i,j,k,l,m;
} test_t;
#endif

#ifdef TESTALLIGNED
typedef struct
{
	int a[1024];
} pageDir_t;
#endif

#ifdef TESTA
void wait()
{
	int i;
	for (i = 0; i < 0x1FFFFFFF; i++)
	{
	}
}
#endif

#include <text.h>
#include <types.h>
#include <mm/memory.h>
#include <mm/heap.h>

#define K128 0x8000000

#ifdef TESTALLOC
memNode_t* findHdr(void* ptr)
{
	memNode_t* tmp = (void*)ptr - sizeof(memNode_t);
	return tmp;
}
#endif

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
#ifdef TESTALLOC
	examineHeap();
	test_t* a = kalloc(sizeof(test_t));
	test_t* b = kalloc(sizeof(test_t));
	test_t* c = kalloc(sizeof(test_t));
	
	printf("a\t\tb\t\tc\t\tsize\n");
	printhex((int)(void*)a); putc('\t');
	printhex((int)(void*)b); putc('\t');
	printhex((int)(void*)c); putc('\t');
	printhex(sizeof(test_t)); putc('\n');
	
	printf("\na\tb\tc\tsize\n");
	printhex(findHdr(a)->size); putc('\t');
	printhex(findHdr(b)->size); putc('\t');
	printhex(findHdr(c)->size); putc('\t');
	printhex(sizeof(test_t)); putc('\n');
	wait();
	free(a);
	wait();
	free(b);
	wait();
	free(c);
	a = kalloc(sizeof(test_t));
	free (a);
	wait();
	
	examineHeap();
#endif
#ifdef TESTALLIGNED
	pageDir_t* a = alloc(sizeof(pageDir_t), TRUE);
	pageDir_t* b = alloc(sizeof(pageDir_t), TRUE);
	pageDir_t* c = alloc(sizeof(pageDir_t), TRUE);
	examineHeap();
	printf("a\t\tb\t\tc\t\tsize\n");
	printhex((int)(void*)a); putc('\t');
	printhex((int)(void*)b); putc('\t');
	printhex((int)(void*)c); putc('\t');
	printhex(sizeof(pageDir_t)); putc('\n');
	printf("1\n");
	wait();
	free(a);
	wait();
	printf("2\n");
	free(b);
	wait();
	printf("3\n");
	free(c);
	wait();
	printf("4\n");
	a = alloc(sizeof(pageDir_t), TRUE);
	free (a);
	
	examineHeap();
#endif
	//installInterruptVectorTable();
	//initPaging();
	//exec(decompress(gzipped kernel));

	for (;;); // Prevent the CPU from
		  // doing stupid things.
	return 0;
}
