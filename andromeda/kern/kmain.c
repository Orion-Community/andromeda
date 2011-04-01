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
} test_t; // This is made to test memory allocation with small pieces of data.
#endif

#ifdef TESTALLIGNED
typedef struct
{
	int a[1024];
} pageDir_t; // This is made to test the memory allocation with larger pieces of data
#endif

#ifdef TESTA
void wait()
{
	int i;
	for (i = 0; i < 0x1FFFFFFF; i++)
	{
	}
} // Made to make the system wait a few seconds, will be replaced once a timer is in place
#endif

// Basic includes
#include <text.h>
#include <types.h>
#include <mm/memory.h>

// Define the place of the heap
#ifdef __COMPRESSED
#define HEAP 0x8000000
#define HEAPSIZE HEAP-0x8000
#else
#define HEAP 0xE0000000
#define HEAPSIZE 10000000
#endif

// This is necessary for finding certain pointers in the memory headers
#ifdef TESTALLOC
memNode_t* findHdr(void* ptr)
{
	memNode_t* tmp = (void*)ptr - sizeof(memNode_t);
	return tmp;
}
#endif

// Print a welcome message
void announce()
{
	textInit();
	println("Compressed kernel loaded");
	println("Decompressing the kernel");
}

// The main function
int kmain(/* boot data , boot data , gzipped kernel*/)
{
	// Initialise the heap
	initHeap(HEAP, HEAPSIZE);
	// If in the compressed image
	#ifdef __COMPRESSED
	announce(); // print welcome message
	#ifdef __INTEL
	// Intel specific function
	setGDT();  // Also in decompressed kernel as the compressed image could be overwritten
	#endif
	#ifdef TESTALLOC
	examineHeap();
	test_t* a = kalloc(sizeof(test_t)); // Make some values to test with
	test_t* b = kalloc(sizeof(test_t));
	test_t* c = kalloc(sizeof(test_t));
	
	printf("a\t\tb\t\tc\t\tsize\n");
	printhex((int)(void*)a); putc('\t'); // Print the pointer addresses
	printhex((int)(void*)b); putc('\t');
	printhex((int)(void*)c); putc('\t');
	printhex(sizeof(test_t)); putc('\n');
	
	printf("\na\tb\tc\tsize\n");
	printhex(findHdr(a)->size); putc('\t'); // Print the sizes of the blocks
	printhex(findHdr(b)->size); putc('\t');
	printhex(findHdr(c)->size); putc('\t');
	printhex(sizeof(test_t)); putc('\n');

	#endif
	#ifdef TESTALLIGNED
	pageDir_t* d = alloc(sizeof(pageDir_t), TRUE); // Make some values to test with, these should be page alligned
	pageDir_t* e = alloc(sizeof(pageDir_t), TRUE);
	pageDir_t* f = alloc(sizeof(pageDir_t), TRUE);
	examineHeap();
	printf("a\t\tb\t\tc\t\tsize\n");
	printhex((int)(void*)d); putc('\t'); // Print the size
	printhex((int)(void*)e); putc('\t');
	printhex((int)(void*)f); putc('\t');
	printhex(sizeof(pageDir_t)); putc('\n');
	#endif
	#ifdef TESTALLOC
// 	wait();
	free(a); // Free the values and see the output
// 	wait();
	free(b);
// 	wait();
	free(c);
	a = kalloc(sizeof(test_t)); // Do a test allocation
	free (a);
// 	wait();
	
	examineHeap(); // Show the heap
	#endif
	#ifdef TESTALLIGNED
	printf("1\n"); // Free the values and see the output
// 	wait();
	free(d);
// 	wait();
	printf("2\n");
	free(e);
// 	wait();
	printf("3\n");
	free(f);
// 	wait();
	printf("4\n");
	d = alloc(sizeof(pageDir_t), TRUE); // Do a test allocation
	free (d);
	
	examineHeap(); // Show the heap
	#endif
	#endif
	//installInterruptVectorTable();
	//initPaging();
	#ifndef __COMPRESSED
	//startInterrupts(); // Interrupts are allowed again.
			     // Up untill this point they have
			     // been disabled.
	#else
	//exec(decompress(gzipped kernel));
	#endif
	for (;;) // Infinite loop, to prevent the CPU from executing rubbish
	{
		#ifndef __COMPRESSED
		// If this loop gets reached more than once:
		sched();
		#endif
	}
	return 0; // To keep the compiler happy.
}
