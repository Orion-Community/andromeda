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

#ifdef FAST
void memset(void *dest, int sval, size_t count)
{
  if(!count){return;}
#ifdef X86 //64 bit int is only faster at X86, X64 prefers 2 time 32 int
  sval &= 0x000000ff;
  unsigned long long val = (unsigned long long)sval;
  char i = 8;
  for(;i<64;i+=8)
  {
    val |= (sval << i);
  }
  while(count >= 8){ *(unsigned long long*)dest = (unsigned long long)val; dest += 8; count -= 8; }
  if(count >= 4){ *(unsigned int*)dest = (unsigned int)val; dest += 4; count -= 4; }
#else
  unsigned int val = (unsigned int)sval;
  char i = 8;
  for(;i<32;i+=8)
  {
    val |= (sval << i);
  }
  while(count >= 4){ *(unsigned int*)dest = (unsigned int)val; dest += 4; count -= 4; }
#endif
  if(count >= 2){ *(unsigned short*)dest = (unsigned short)val; dest += 2; count -= 2; }
  if(count >= 1){ *(unsigned char*)dest = (unsigned char)val; }
  return; 
}

void memcpy(void *dest, void *src, size_t count)
{
  if(!count){return;}
#ifdef X86 //64 bit int is only faster at X86, X64 prefers 2 time 32 int
  if( ABS( (int)(dest - src) ) >= 8)
    while(count >= 8){ *(unsigned long long*)dest = *(unsigned long long*)src; dest += 8; src += 8; count -= 8; }
  if( (ABS( (int)(dest - src) ) >= 4) && (count >= 4) ) { *(unsigned int*)dest = *(unsigned int*)src; dest += 4; src += 4; count -= 4; }
#else
  if( ABS( (int)(dest - src) ) >= 4)
    while(count >= 4){ *(unsigned int*)dest = *(unsigned int*)src; dest += 4; src += 4; count -= 4; }
#endif
  if( (ABS( (int)(dest - src) ) >= 2) && (count >= 2) ) { *(unsigned short*)dest = *(unsigned short*)src; dest += 2; src += 2; count -= 2; }
  if( (ABS( (int)(dest - src) ) >= 1) && (count = 1) ) { *(unsigned char*)dest = *(unsigned char*)src; }
  return;
}

int memcmp(void *ptr1, void* ptr2, size_t count)
{
#ifdef X86 //64 bit int is only faster at X86, X64 prefers 2 time 32 int
  if( ABS( (int)(ptr1 - ptr2) ) >= 8)
    while(count >= 8){ if(*(unsigned long long*)ptr1 - *(unsigned long long*)ptr2) return 1; ptr1 += 8; ptr2 += 8; count -= 8; }
  if( (ABS( (int)(ptr1 - ptr2) ) >= 4) && (count >= 4) ) { if(*(unsigned int*)ptr1 - *(unsigned int*)ptr2) return 1; ptr1 += 4; ptr2 += 4; count -= 4; }
#else
  if( ABS( (int)(ptr1 - ptr2) ) >= 4)
    while(count >= 4){ if(*(unsigned int*)ptr1 - *(unsigned int*)ptr2) return 1; ptr1 += 4; ptr2 += 4; count -= 4; }
#endif
  if( (ABS( (int)(ptr1 - ptr2) ) >= 2) && (count >= 2) ) { if(*(unsigned short*)ptr1 - *(unsigned short*)ptr2) return 1; ptr1 += 2; ptr2 += 2; count -= 2; }
  if( (ABS( (int)(ptr1 - ptr2) ) >= 1) && (count = 1) ) { if(*(unsigned char*)ptr1 - *(unsigned char*)ptr2) return 1; }
  return 0;
}

#else
void memset(void* location, int value, size_t size)
{
  int i = 0;
  unsigned char* offset = (unsigned char*)location;
  for (; i < size; i++)
  {
    *(offset+i) = (unsigned char)value;
  }
}

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
#endif

size_t strlen(char* string)
{
  int i = 0;
  for (; *(string+i) != '\0'; i++);
  return i;
}
