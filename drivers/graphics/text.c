/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Steven van der Schoot

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
 * 
 * THIS FILE IS JUST A STUB!
 * Please don't implement jet. (nothing is tested!)
 * 
 */

#include <stdlib.h>
#include <io.h>
#include <text.h>
#include "include/graphics.h"
#include "include/TTY.h"

void textInit()
{
#ifdef HD
  /*
   * Note that the code isn't HD ready jet. Only low resolustion VGA is allowed!
   * 
   *   //  Set videomode (assambly) for real mode:  //
   *   //    AX = 4F02h                             //
   *   //    BX =  11Bh                             //
   *   //    int 10                                 //
   *   //  Returns:                                 //
   *   //    AL = 4Fh if function supported         //
   *   //    AH = status (0=succes, 1=failt)        //
   * 
   */
  graphicsInit(1280,1024,3/*=16M colors*/); // Will fix this later for wide-screens...
#else
  /*
   *   //  Set videomode (assambly) for real mode:  //
   *   //    AX = 00h                               //
   *   //    BX = 0Dh                               //
   *   //    int 10                                 //
   */
  graphicsInit(320,200,1);
#endif
  ttyInit();
}

char hex[36] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char HEX[36] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

  /*
   * remake of the nano kernels printNum. Optemized for buffer use + support long number (nano kernel version only supports up to 32 charakters)
   * 
   * Function has been tested and works fine!
   * 
   */
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

  /*
   * Function has been tested and works fine!
   */
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

  return count+1;
}

  /*
   * TODO: ftoa function!
   */

unsigned int formatBuf(void *buffer, unsigned char *format, ...)
{
  int i;
  va_list list;
  unsigned int tmp;
  va_start(list, format);
  for (i = 0; format[i] != '\0'; i++)
  {
    if (format[i] != '%')
    {
      *((unsigned char*)buffer++) = format[i];
    }
    else
    {
      i++;
      if (format[i+1] == '\0')
      {
	*((unsigned char*)buffer++) = format[i];
	break;
      }
      switch(format[i])
      {
	case 'd':
	case 'i':
	  buffer += itoa(buffer, va_arg(list, unsigned int), 10, TRUE, FALSE);
	  break;
	case 'u':
	  buffer += itoa(buffer, va_arg(list, unsigned int), 10, FALSE, FALSE);
	  break;
	case 'x':
	  buffer += itoa(buffer, va_arg(list, unsigned int), 16, FALSE, FALSE);
	  break;
	case 'X':
	  buffer += itoa(buffer, va_arg(list, unsigned int), 16, FALSE, TRUE);
	  break;
	case 'd':
	  buffer += dtoa(buffer, va_arg(list,       double), 10,        FALSE);
	  break;
	case 'c':
	  *((unsigned char*)buffer++) = (unsigned char)va_arg(list, unsigned int);
	  break;
	case 's':
          char* str = va_arg(list, char*);
          int len = strlen(str);
	  memcpy( buffer, str, strlen(str) );
          buffer += len;
	  break;
	case '%':
	  *((unsigned char*)buffer++) = '%';
	  break;
	default:
	  break;
      }
    }
    va_end(list);
  }
  return (unsigned int)buffer;
}

void fprintf(void *buffer, unsigned char *format, ...)
{
  va_list list;
  va_start(list, format);
  formatBuf(buffer, format, list/* <- Don't know for sure if this works*/);
}

void printf(unsigned char *format, ...)
{
  va_list list;
  va_start(list, format);
  ttyPtr -= (int)(ttyBuf + ttyPtr) - (int)formatBuf(ttyBuf + ttyPtr, format, list/* <- Don't know for sure if this works*/);
}
