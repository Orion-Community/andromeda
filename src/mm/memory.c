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

/* memcpy from the andromeda project */
void ol_memcpy(void *dest, void *src, size_t count)
{
#ifndef X86 //64 bit int is only faster at 64-bit PC's, 32 bits prefers 2 time 32 int
  while(count >= 8)
  {
    *(unsigned long long*)dest = *(unsigned long long*)src;
    dest += 8;
    src += 8;
    count -= 8;
  }
  if(count >= 4)
  {
    *(unsigned long long*)dest = *(unsigned long long*)src;
    dest += 4;
    src += 4;
    count -= 4;
  }
#else
  while(count >= 4)
  {
    *(unsigned int*)dest = *(unsigned int*)src;
    dest += 4;
    src += 4;
    count -= 4;
  }
#endif
  if(count >= 2)
  {
    *(unsigned long long*)dest = *(unsigned long long*)src;
    dest += 2;
    src += 2;
    count -= 2;
  }
  if(count >= 1)
  {
    *(unsigned long long*)dest = *(unsigned long long*)src;
  }
  return;
}
