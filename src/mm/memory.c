/*
    Andromeda
    Copyright (C) 2011, 2012
    Bart Kuivenhoven, Steven van der Schoot, Michel Megens

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
#include <mm/paging.h>
#include <mm/map.h>
#include <mm/heap.h>

#ifndef SLAB
#define BASE_HEAP_SIZE 0x40000

int heap_base[BASE_HEAP_SIZE]; // Heap base = 4 MiB

int
complement_heap(void* base, size_t size)
{
        heap_add_blocks(base, size);
        return 0;
}

int
init_heap()
{
        heap_add_blocks(&heap_base, BASE_HEAP_SIZE);
        freeable_allocator = TRUE;
        return -E_SUCCESS;
}
#endif
void memset(void *dest, int sval, size_t count)
{
        if (!count)
        {
                return;
        }
        sval &= 0x000000ff;
#ifndef X86 //64 bit int is only faster at X86, X64 prefers 2 time 32 int
        unsigned long long val = (unsigned long long) sval;
        char i = 8;
        for (; i < 64; i += 8)
        {
                val |= (sval << i);
        }
        while (count >= 8)
        {
                *(unsigned long long*) dest = (unsigned long long) val;
                dest += 8;
                count -= 8;
        }
        if (count >= 4)
        {
                *(unsigned int*) dest = (unsigned int) val;
                dest += 4;
                count -= 4;
        }
#else
        unsigned int val = (unsigned int) sval;
        char i = 8;
        for (; i < 32; i += 8)
        {
                val |= (sval << i);
        }
        while (count >= 4)
        {
                *(unsigned int*) dest = (unsigned int) val;
                dest += 4;
                count -= 4;
        }
#endif
        if (count >= 2)
        {
                *(unsigned short*) dest = (unsigned short) val;
                dest += 2;
                count -= 2;
        }
        if (count >= 1)
        {
                *(unsigned char*) dest = (unsigned char) val;
        }
        return;
}

// void memset(void* var, int val, size_t count)
// {
//         for (;count != 0; count --)
//         {
//                 *(char*)((long)var+count) = (char) val;
//         }
// }

void memcpyBack(void *dest, void *src, size_t count)
{
        dest += count;
        src  += count;
#ifndef X86
//64 bit int is only faster at 64-bit PC's, 32 bits prefers 2 time 32 int
        while (count >= 8)
        {
                *(unsigned long long*) dest = *(unsigned long long*) src;
                dest -= 8;
                src -= 8;
                count -= 8;
        }
        if (count >= 4)
        {
                *(unsigned long long*) dest = *(unsigned long long*) src;
                dest -= 4;
                src -= 4;
                count -= 4;
        }
#else
        while (count >= 4)
        {
                *(unsigned int*) dest = *(unsigned int*) src;
                dest -= 4;
                src -= 4;
                count -= 4;
        }
#endif
        if (count >= 2)
        {
                *(unsigned short*) dest = *(unsigned short*) src;
                dest -= 2;
                src -= 2;
                count -= 2;
        }
        if (count >= 1)
        {
                *(unsigned char*) dest = *(unsigned char*) src;
        }
        return;
}

void memcpy(void *dest, void *src, size_t count)
{
        if ((int)src+count>(int)dest)
                memcpyBack(dest,src,count);
#ifndef X86
        //64 bit int is only faster at 64-bit PC's, 32 bits prefers 2 time 32 int
        while (count >= 8)
        {
                *(unsigned long long*) dest = *(unsigned long long*) src;
                dest += 8;
                src += 8;
                count -= 8;
        }
        if (count >= 4)
        {
                *(unsigned long long*) dest = *(unsigned long long*) src;
                dest += 4;
                src += 4;
                count -= 4;
        }
#else
        while (count >= 4)
        {
                *(unsigned int*) dest = *(unsigned int*) src;
                dest += 4;
                src += 4;
                count -= 4;
        }
#endif
        if (count >= 2)
        {
                *(unsigned short*) dest = *(unsigned short*) src;
                dest += 2;
                src += 2;
                count -= 2;
        }
        if (count >= 1)
        {
                *(unsigned char*) dest = *(unsigned char*) src;
        }
        return;
}

int memcmp(void *ptr1, void* ptr2, size_t count)
{
#ifndef X86
        //64 bit int is only faster at X86, X64 prefers 2 time 32 int
        while (count >= 8)
        {
                if (*(unsigned long long*) ptr1 - *(unsigned long long*) ptr2)
                        return 1;
                ptr1 += 8;
                ptr2 += 8;
                count -= 8;
        }
        if (count >= 4)
        {
                if (*(unsigned long long*) ptr1 - *(unsigned long long*) ptr2)
                        return 1;
                ptr1 += 4;
                ptr2 += 4;
                count -= 4;
        }
#else
        while (count >= 4)
        {
                if (*(unsigned int*) ptr1 - *(unsigned int*) ptr2)
                        return 1;
                ptr1 += 4;
                ptr2 += 4;
                count -= 4;
        }
#endif
        while(count >= 2)
        {
                if (*(unsigned short*) ptr1 - *(unsigned short*) ptr2)
                        return 1;
                ptr1 += 2;
                ptr2 += 2;
                count -= 2;
        }
        if ((count == 1)&&(*(unsigned char*) ptr1 - *(unsigned char*) ptr2))
        {
                return 1;
        }
        return 0;
}

size_t strlen(char* string)
{
        int i = 0;
        for (; *(string + i) != '\0'; i++);
        return i;
}

/** \file */
