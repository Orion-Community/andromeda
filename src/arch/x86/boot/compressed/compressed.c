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

#include <text.h>
#include <types.h>
#include <mm/memory.h>
#include <mm/heap.h>

#define K128 0x8000000

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
	//installInterruptVectorTable();
	//initPaging();
	//exec(decompress(gzipped kernel));

	for (;;); // Prevent the CPU from
		  // doing stupid things.
	return 0;
}
