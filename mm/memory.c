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

void memset(void* location, int value, size_t size)
{
  int i = 0;
  unsigned char* offset = (unsigned char*)location;
  for (; i < size; i++)
  {
    *(offset+i) = (unsigned char)value;
  }
}

/*
 * Fast version:
 */
void memcpy(void *dest, void *src, size_t count)
{
  if(!count){return;}
#ifdef X86
  while(count >= 8){ *(unsigned long long*)dest = *(unsigned long long*)src; memcpy_transfers_64++; dest += 8; src += 8; count -= 8; }
#endif
  while(count >= 4){ *(unsigned int*)dest = *(unsigned int*)src; memcpy_transfers_32++; dest += 4; src += 4; count -= 4; }
  while(count >= 2){ *(unsigned short*)dest = *(unsigned short*)src; memcpy_transfers_16++; dest += 2; src += 2; count -= 2; }
  while(count >= 1){ *(unsigned char*)dest = *(unsigned char*)src; memcpy_transfers_8++; dest += 1; src += 1; count -= 1; }
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

/*
 * Fast version:
 */
void memset(void *dest, uint8 sval, size_t count)
{
  unsigned long long val = (sval & 0xFF);
  val |= ((val << 8) & 0xFF00);
  val |= ((val << 16) & 0xFFFF0000);
  val |= ((val << 32) & 0xFFFFFFFF00000000);
    
  if(!count){return;}
#ifdef X86
  while(count >= 8){ *(unsigned long long*)dest = (unsigned long long)val; memset_transfers_64++; dest += 8; count -= 8; }
#endif
  while(count >= 4){ *(unsigned int*)dest = (unsigned int)val; memset_transfers_32++; dest += 4; count -= 4; }
  while(count >= 2){ *(unsigned short*)dest = (unsigned short)val; memset_transfers_16++; dest += 2; count -= 2; }
  while(count >= 1){ *(unsigned char*)dest = (unsigned char)val; memset_transfers_8++; dest += 1; count -= 1; }
  return; 
}

/*
 * Old one:
 *
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
*/

size_t strlen(char* string)
{
  int i = 0;
  for (; *(string+i) != '\0'; i++);
  return i;
}
