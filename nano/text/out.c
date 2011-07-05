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
#include <io.h>
#include "include/out.h"

struct curPos cursor = {0,0,16};
char col = 0x07;

void textInit()
{
	scroll(HEIGHT);
}

void scroll(unsigned char lines)
{
	int x, y;
	unsigned short *keybuf = (unsigned short *)KEYBUF;
	for (y = 0; y < HEIGHT-lines; y++)
	{
		for (x=0; x < WIDTH; x++)
		{
			keybuf[x+y*WIDTH] = keybuf[x+(y+lines)*WIDTH];
		}
	}
	for (; y < HEIGHT; y++)
	{
		for (x = 0; x< WIDTH; x++)
		{
			keybuf[x+y*WIDTH] = ((col << 8) | ' ');
		}
	}
	if (lines >= HEIGHT)
	{
		cursor.x=0;
		cursor.y=0;
		return;
	}
	cursor.y-=lines;
}

void println(unsigned char *line)
{
	printf(line);
	putc('\n');
}

void printf(unsigned char *line, ...)
{
  int i;
  va_list list;
  unsigned int tmp;
  va_start(list, line);
  for (i = 0; line[i] != '\0'; i++)
  {
    if (line[i] != '%')
    {
      putc(line[i]);
    }
    else
    {
      i++;
      if (line[i+1] == '\0')
      {
	putc(line[i]);
	break;
      }
      switch(line[i])
      {
	case 'd':
	  printDecimalNum(va_arg(list, double), 10, FALSE);
	  break;
	case 'i':
	  printNum(va_arg(list, unsigned int), 10, TRUE, FALSE);
	  break;
	case 'u':
	  printNum(va_arg(list, unsigned int), 10, FALSE, FALSE);
	  break;
	case 'x':
	  printNum(va_arg(list, unsigned int), 16, FALSE, FALSE);
	  break;
	case 'X':
	  printNum(va_arg(list, unsigned int), 16, FALSE, TRUE);
	  break;
	case 'c':
	  putc((char)va_arg(list, unsigned int));
	  break;
	case 's':
	  printf(va_arg(list, char*));
	  break;
	case '%':
	  putc('%');
	  break;
	default:
	  break;
      }
    }
    va_end(list);
  }
}

char hex[36] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char HEX[36] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

int itoa(void* buffer, int num, unsigned int base, boolean sInt, boolean capital)
{
  int bufBase = (int)buffer;
  if (base > 36 || base < 2 )
    return 0;
  int count = 0;
  if(num < 0)
  {
    if(sInt)
    {
      *((char*)buffer++) = '-';
      count++;
    }
    num = -num;
  }
  int i = base, buf;
  while(num>=i)
  {
    i *= base;
    count++;
  }
  for(i/=base;i>=1;i/=base)
  {
    buf = (int)(num/i);
    *((char*)buffer++) = (capital)? HEX[buf] : hex[buf];
    num -= buf*i;
  }
  return count+1;
}


int dtoa(void* buffer, double num, unsigned int base, boolean capital)
{
  if (base > 36 || base < 2 )
    return 0;

  int count = itoa(buffer,(int)num, base, TRUE, capital); // print part before '.'

  double decimals = num-(double)((int)num);
  if(decimals==0) // check for xx.0
    return count;

  buffer+=count;
  *((char*)buffer++) = '.';

  for (;decimals>0;count++)
  {
    decimals*=10;
    *((char*)buffer++) = (capital)? HEX[(int)decimals] : hex[(int)decimals];
    decimals-=(double)((int)decimals);
  }

  return count+1; // +1 because of the dot character
}

void printNum(int index, unsigned int base, boolean sInt, boolean capital)
{
  /*
   * Old version:
   * 
  char buf[32];
  memset(buf, '\0', 32);
  int i = 0;
  
  if (base > 36 || base < 2 )
    return;
  if (index == 0)
    putc('0');
  
  if (index < 0 && sInt)
  {
    putc('-');
    index *= -1;
  } 
  unsigned int uIndex = (unsigned int) index;
  for (; uIndex != 0; i++)
  {
    buf[31-i] = (capital) ? HEX[uIndex%base] : hex[uIndex%base];
    uIndex /=base;
  }
  for (i--; i >= 0; i--)
  {
    putc(buf[31-i]);
  }
   * 
   * New version:
   */
  char buf[32];
  memset(buf,'\0',32);
  int i = 0,count = itoa(buf,index, base, sInt,capital);
  for (;i<count;i++)
  {
    putc(buf[i]);
  }
}

void printDecimalNum(double index, unsigned int base, boolean capital)
{
  char buf[64];
  memset(buf,'\0',64);
  int i = 0,count = dtoa(buf,index, base,capital);
  for (;i<count;i++)
  {
    putc(buf[i]);
  }
}

void putc(unsigned char i)
{
	boolean noPrint = FALSE;
	if (i == '\n')
	{
		cursor.y++;
		cursor.x = 0;
		noPrint = TRUE;
	}

	if (i == '\t')
	{
		int i;
		if (cursor.x % cursor.tabwidth == 0)
		{
			putc(' ');
		}
		for (i = 0; i < cursor.x%cursor.tabwidth; i++)
		{
			putc(' ');
		} 
		noPrint = TRUE;
	}
	if (cursor.x+1 > WIDTH)
	{
		cursor.x %= WIDTH;
		cursor.y += 1;
	}
	if (cursor.y >= HEIGHT)
	{
		scroll(cursor.y % HEIGHT+1);
	}
	if (!noPrint)
	{
		cursor.x++;
		unsigned short *keybuf = (unsigned short *)KEYBUF;
		unsigned short chr = (unsigned short)(col << 8) | (unsigned short)(i & 0x00FF);
		keybuf[cursor.x-1+cursor.y*WIDTH] = chr;
	}
	reloc(cursor.x, cursor.y);
}

int reloc(int loc_x, int loc_y)
{
        /*
         * Set the new cursor position
         */
        unsigned short location = loc_y * WIDTH + loc_x;
        outb(0x3D4, 14); 
        outb(0x3D5, location >> 8);
        outb(0x3D4, 15);
        outb(0x3D5, location);
        return 0;
}
