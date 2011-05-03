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

#include <mm/memory.h>
#include <mm/heap.h>
#include <error/panic.h>
#include <text.h>
#include <thread.h>

extern memNode_t* blocks;
extern mutex_t prot;
int growHeap()
{
}

// This initialises the heap to hold a block of the maximum possible size.
// In the case of the compressed kernel that's 128 MB, which is huge, since
// allocmax = 4KB
void initBlockMap ()
{
	mutexEnter(prot);
	memNode_t* node = (memNode_t*)heapBase;
	initHdr(node, heapSize-sizeof(memNode_t));
	blocks = node;
	mutexRelease(prot);
	#ifdef MMTEST
	testAlloc();
	#endif
}