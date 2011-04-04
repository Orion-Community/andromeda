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

// Basic includes
#include <text.h>
#include <types.h>
#include <mm/memory.h>
#include <interrupts/int.h>

// Define the place of the heap
#ifdef __COMPRESSED
#define HEAP 0x8000000
#define HEAPSIZE HEAP-0x8000
#else
#define HEAP 0xE0000000
#define HEAPSIZE 10000000
#endif
#define VENDOR_INTEL	1
#define VENDOR_AMD 	2
#include <kern/cpu.h>

int vendor = 0;

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
	#ifdef VENDORTELL
	printf("Hello, world!\n");
	#endif
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
	
	#ifdef VENDORTELL
	switch(getVendor())
	{
	  case VENDOR_INTEL:
	    printf("You're using a Genuine Intel\n");
	    break;
	  case VENDOR_AMD:
	    printf("You're using an authentic AMD\n");
	    break;
	  default:
	    printf("You're using a system not officially supported\n");
	}
	#endif
	
	//installInterruptVectorTable();
	//initPaging();
	intInit(); 	     // Interrupts are allowed again.
			     // Up untill this point they have
			     // been disabled.
	#ifdef __COMPRESSED
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
