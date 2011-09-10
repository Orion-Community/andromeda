/*
 *   Memory functions
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <textio.h>

/* memcpy from the andromeda project */
void
ol_memcpy(void *dest, void *src, size_t count)
{
#ifndef X86 //64 bit int is only faster at 64-bit PC's, 32 bits prefers 2 time 32 int
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
                *(unsigned long long*) dest = *(unsigned long long*) src;
                dest += 2;
                src += 2;
                count -= 2;
        }
        if (count >= 1)
        {
                *(unsigned long long*) dest = *(unsigned long long*) src;
        }
        return;
}

int
memcmp(void *ptr1, void* ptr2, size_t count)
{
#ifndef X86 // x64 prefers longs and x86 prefers ints
        if (diff >= 8)
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
        if (count >= 2)
        {
                if (*(unsigned long long*) ptr1 - *(unsigned long long*) ptr2)
                        return 1;
                
                ptr1 += 2;
                ptr2 += 2;
                count -= 2;
        }
        if ((count == 1) && (*(unsigned long long*) ptr1 - *(unsigned long long*) ptr2))
        {
                return 1;
        }
        return 0;
}

size_t
strlen(char * str)
{
        size_t i = 0;
        while(str[i] != '\0') i++;
        return i;
}
