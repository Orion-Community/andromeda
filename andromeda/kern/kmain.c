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
 * This is the main function for both the compressed and decompressed image.
 * The compressed image is nothing more than a basic kernel with image loading
 * code, to load the decompressed image. An approach still has to be chosen on
 * how to load the decompressed image.
 *
 * One angle is to load the decompressed image in from disk.
 * Another is to get a gzipped image from the bootloader and decompress that
 * and use that as the decompressed image.
 * The latter is the most gracefull, however, the former will suffice and is
 * probably a whole lot easier.
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
