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

#include <stdlib.h>
#include <mm/paging.h>
#include <mm/map.h>

int initHeap(long size)
{
  long done = 0;
  int i = 0x100;
  for (; i < PAGES; i++)
  {
    if (bitmap[i] == FREE)
    {
      heapAddBlocks((void*)(i*PAGESIZE), PAGESIZE);
      done += PAGESIZE;
      claimPage((long)i, COMPRESSED);
      if (size <= done)
      {
	break;
      }
    }
  }
  #ifdef DBG
  examineHeap();
  #endif
  initPaging();

  return 0;
}

/*
 * Fast version:
 */
void memset(void *dest, int val, size_t count)
{
  if(!count){return;}
#ifdef X86
  long long valbuf = (val | ((val << 32) & & 0xffffffff00000000))
  while(count >= 8){ *(unsigned long long*)dest = (unsigned long long)valbuf; dest += 8; count -= 8; }
  free(valbuf);
  if(count >= 4){ *(unsigned int*)dest = (unsigned int)val; dest += 4; count -= 4; }
#else
  while(count >= 4){ *(unsigned int*)dest = (unsigned int)val; dest += 4; count -= 4; }
#endif
  if(count >= 2){ *(unsigned short*)dest = (unsigned short)val; dest += 2; count -= 2; }
  if(count >= 1){ *(unsigned char*)dest = (unsigned char)val; }
  return; 
}

/*
 * Old one:
 *
void memset(void* location, int value, size_t size)
{
  int i = 0;
  unsigned char* offset = (unsigned char*)location;
  for (; i < size; i++)
  {
    *(offset+i) = (unsigned char)value;
  }
}
*/

/*
 * Fast version:
 */
void memcpy(void *dest, void *src, size_t count)
{
  if(!count){return;}
#ifdef X86
  while(count >= 8){ *(unsigned long long*)dest = *(unsigned long long*)src; dest += 8; src += 8; count -= 8; }
  if(count >= 4){ *(unsigned int*)dest = *(unsigned int*)src; dest += 4; src += 4; count -= 4; }
#else
  while(count >= 4){ *(unsigned int*)dest = *(unsigned int*)src; dest += 4; src += 4; count -= 4; }
#endif
  if(count >= 2){ *(unsigned short*)dest = *(unsigned short*)src; dest += 2; src += 2; count -= 2; }
  if(count = 1){ *(unsigned char*)dest = *(unsigned char*)src; }
  return;
}

/*
 * Old one:
 *
void memcpy(void *destination, void* source, size_t num)
{
  int i = 0;
  unsigned char* src = source;
  unsigned char* dst = destination;
  for (; i < num; i++)
  {
    *(dst+i) = *(src+i);
  }
}
*/

int memcmp(void *ptr1, void* ptr2, size_t num)
{
  int ret = 0;
  int i = 0;
  for (; i < num && ret == 0; i++)
  {
    ret = *((unsigned char*)ptr1+i) - *((unsigned char*)ptr2+i);
  }
  return ret;
}

size_t strlen(char* string)
{
  int i = 0;
  for (; *(string+i) != '\0'; i++);
  return i;
}
