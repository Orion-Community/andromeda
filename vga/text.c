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
 * 
 * THIS FILE IS JUST A STUB!
 * Please don't implement jet.
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
   * Note that the code isn't HD ready jet. Only low res. VGA is allowed.
   */
  graphicsInit(???,???,???);
#else
  graphicsInit(320,200,1);
#endif
  ttyInit();
}

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
	  printNum(va_arg(list, unsigned int), 10, TRUE, FALSE); //needs to be replaced!
	  break;
	case 'u':
	  printNum(va_arg(list, unsigned int), 10, FALSE, FALSE); //needs to be replaced!
	  break;
	case 'x':
	  printNum(va_arg(list, unsigned int), 16, FALSE, FALSE); //needs to be replaced!
	  break;
	case 'X':
	  printNum(va_arg(list, unsigned int), 16, FALSE, TRUE);  //needs to be replaced!
	  break;
	case 'c':
	  *((unsigned char*)buffer++) = (unsigned char)va_arg(list, unsigned int);
	  break;
	case 's':
	  printf(va_arg(list, char*));
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
  formatBuf(buffer, format, list/*Don't know for sure if this works*/);
}

void printf(unsigned char *format, ...)
{
  va_list list;
  va_start(list, format);
  ttyPtr -= (int)(ttyBuf + ttyPtr) - (int)formatBuf(ttyBuf + ttyPtr, format, list/*Don't know for sure if this works*/);
}
