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

void vgaInit()
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
  graphicsInit(320,200,1/*=256 colors*/);
#endif
  ttyInit();
}

extern char *hex;
extern char *HEX;

int formatInt(void* buffer, int num, unsigned int base, boolean sInt, boolean capital)
{
  int bufBase = (int)buffer;
  if (base > 36 || base < 2 )
    return 0;
  int count = 0;
  if( (num < 0) && sInt) //if number is negative print '-' and print absolue value of number (witch is -num)
  {
    *((char*)buffer++) = '-';
    count++;
    num = -num;
  }
  unsigned int i = base, numBase = num/base, buf;
  if (numBase>0)
  {
    while(i<=numBase)
    {
      i *= base;
      count++;
    }
  }
  else
  {
    count--;
    i = 1;
  }
  for(;i>=1;i/=base)
  {
    buf = (unsigned int)(num/i);
    *((char*)buffer++) = (capital)? HEX[buf] : hex[buf];
    num -= buf*i;
  }
  return count+2;
}

char* itoa(unsigned int index, char* buffer, unsigned int base)
{
  int len = formatInt(buffer, index, base, (base==10)?TRUE:FALSE, FALSE); // only signed if base = 10
  buffer[len] = '\0';
  return buffer;
}

int atoi(char* str)
{
  int i = 0;
  int idx = 0;
  while (str[i] != '\0' && idx < 0x20000000) // (idx < 0x20000000) = overload prevention
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      if( 0xffffffff-(idx*10) > (str[i] - 0x30) ) //overload prevention
        idx = idx * 10 + (str[i] - 0x30);
      else
        break;
      i++;
    }
  }
  if (str[0]=='-')
    idx *= -1;
  return idx;
}

long atol(char* str)
{
  int i = 0;
  unsigned long long idx = 0;
  while (str[i] != '\0' && idx < 0x2000000000000000) // (idx < 0x20000000) = overload prevention
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      if( 0xffffffffffffffff-(idx*10) > (str[i] - 0x30) ) //overload prevention
        idx = idx * 10 + (str[i] - 0x30);
      else
        break;
      i++;
    }
  }
  if (str[0]=='-')
    idx *= -1;
  return idx;
}

    /*
     * Function:
     *   This function converts a double to a string into a given buffer.
     * 
     * Params:
     *   - buffer: ______ Pointer to buffer to be written to.
     *   - num: _________ Double that should be converted.
     *   - base: ________ Numerical system (e.g. 10 for normal number format,
     *                    2 for binairy, 16 for hexadecimal...)
     *   - capital: _____ Only effects on base > 10. If true all characters above 9 will be
     *                    uppercase, if false lower case.
     *   - scientific: __ If true this will print 1 digit before dot and all other digits
     *                    after the dot, ending with exx or Exx presenting * base^xx.
     * 
     * Return:
     *   length of string writen to buffer.
     * 
     */
int formatDouble(void* buffer, double num, unsigned int base, boolean capital, boolean scientific)
{
  if (base > 36 || base < 2 )
    return 0;

  int precision = 0, // persents number after e (or E) when printing scientific.
      count = formatInt(buffer,(int)num, base, TRUE, capital); // print part before dot.

  if(scientific)

  {
    precision = count-1;
    if(precision!=0) // if we should print scientific AND we have more than 1 digit before dot.
    {
      memcpy((char*)buffer+2,(char*)buffer+1,count); // all digits after first digit are moved 1 place to left. (e.g. 12345 becomes 122345)
      *((char*)(buffer+1))= '.'; // replace seccond digit with a dot (e.g. 122345 becomes 1.2345)
      count++;
    }
  }
  buffer+=count;
  double decimals = num-(double)((int)num); // get all decimals of the number.
  if(decimals==0) // check for xx.0
  {
    if(scientific)

    {
      *((char*)buffer++) = (capital)?'E':'e'; //print exx or Exx
      if(precision>=0)
        *((char*)buffer++) = '+';  //print leading + (minus is printed by formatInt)
      count += formatInt(buffer, precision, base, TRUE, capital); //print precision
    }
    return count; // appears to be an integer, so we are done now.
  }
  precision += decimals;

  
  if(precision==0) // this is 0 for non scientific AND for scientific, but without printed dot.
    *((char*)buffer++) = '.';

  for (;decimals>0;count++) //loop through all decimals
  {
    decimals*=10; // move current decimal befor to left side of the dot.
    *((char*)buffer++) = (capital)? HEX[(int)decimals] : hex[(int)decimals]; //print decimal.
    decimals-=(double)((int)decimals); //decimal is printed, remove it!
  }

  if(scientific)
  {
    *((char*)buffer++) = (capital)?'E':'e'; //print exx or Exx
    if(precision>=0)
      *((char*)buffer++) = '+';  //print leading + (minus is printed by formatInt)
    count += 1+formatInt(buffer, precision, base, TRUE, capital); //print precision
  }

  return count+1;
}

char* dtoa(double index, char* buffer, unsigned int base)
{
  int len = formatDouble(buffer, index, base, FALSE, FALSE);
  buffer[len] = '\0';
  return buffer;
}

double atod(char* str)
{
  int i = 0;
  double idx = 0;
  while ( (str[i] != '\0') && (str[i] != '.') ) //parse all digits before dot.
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      idx *= 10; // e.g. 123 -> 1230
      idx += str[i] - 0x30; // e.g. 1230 -> 1234 (where 4 come from '4' - 0x30 = 4)
      i++;
    }
  }
  int i2 = 1;
  while (str[i] != '\0') //parse alle digits after dot.
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      i2 *= 0.1;
      idx += (str[i] - 0x30)*i2;
      i++;
    }
  }
  if (str[0]=='-')
    idx *= -1;
  return idx;
}

    /*
     * Function:
     *   This function converts a float to a string into a given buffer.
     * 
     * Params:
     *   - buffer: ______ Pointer to buffer to be written to.
     *   - num: _________ Float that should be converted.
     *   - base: ________ Numerical system (e.g. 10 for normal number format,
     *                    2 for binairy, 16 for hexadecimal...)
     *   - capital: _____ Only effects on base > 10. If true all characters above 9 will be
     *                    uppercase, if false lower case.
     *   - scientific: __ If true this will print 1 digit before dot and all other digits
     *                    after the dot, ending with exx or Exx presenting * base^xx.
     * 
     * Return:
     *   length of string writen to buffer.
     * 
     */
int formatFloat(void* buffer, float num, unsigned int base, boolean capital, boolean scientific)
{
  if (base > 36 || base < 2 )
    return 0;

  int precision = 0, // persents number after e (or E) when printing scientific.
      count = formatInt(buffer,(int)num, base, TRUE, capital); // print part before dot.

  if(scientific)
  {
    precision = count-1;
    if(precision!=0) // if we should print scientific AND we have more than 1 digit before dot.
    {
      memcpy((char*)buffer+2,(char*)buffer+1,count); // all digits after first digit are moved 1 place to left. (e.g. 12345 becomes 122345)
      *((char*)(buffer+1))= '.'; // replace seccond digit with a dot (e.g. 122345 becomes 1.2345)
      count++;
    }
  }
  buffer+=count;
  float decimals = num-(float)((int)num); // get all decimals of the number.
  if(decimals==0) // check for xx.0
  {
    if(scientific)
    {
      *((char*)buffer++) = (capital)?'E':'e'; //print exx or Exx
      if(precision>=0)
        *((char*)buffer++) = '+';  //print leading + (minus is printed by formatInt)
      count += formatInt(buffer, precision, base, TRUE, capital); //print precision
    }
    return count; // appears to be an integer, so we are done now.
  }
  precision += decimals;

  
  if(precision==0) // this is 0 for non scientific AND for scientific, but without printed dot.
    *((char*)buffer++) = '.';

  for (;decimals>0;count++) //loop through all decimals
  {
    decimals*=10; // move current decimal befor to left side of the dot.
    *((char*)buffer++) = (capital)? HEX[(int)decimals] : hex[(int)decimals]; //print decimal.
    decimals-=(float)((int)decimals); //decimal is printed, remove it!
  }

  if(scientific)
  {
    *((char*)buffer++) = (capital)?'E':'e'; //print exx or Exx
    if(precision>=0)
      *((char*)buffer++) = '+';  //print leading + (minus is printed by formatInt)
    count += 1+formatInt(buffer, precision, base, TRUE, capital); //print precision
  }

  return count+1;
}

char* ftoa(float index, char* buffer, unsigned int base)
{
  int len = formatFloat(buffer, index, base, FALSE, FALSE);
  buffer[len] = '\0';
  return buffer;
}

double atof(char* str)
{
  int i = 0;
  float idx = 0;
  while ( (str[i] != '\0') && (str[i] != '.') )
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      idx *= 10;
      idx += str[i] - 0x30;
      i++;
    }
  }
  int i2 = 1;
  while (str[i] != '\0')
  {
    if (str[i] >= 0x30 && str[i] <= 0x39)
    {
      i2 *= 0.1;
      idx += (str[i] - 0x30)*i2;
      i++;
    }
  }
  if (str[0]=='-')
    idx *= -1;
  return idx;
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
	case 'f':
          buffer += formatFloat(buffer, va_arg(list, float), 10, FALSE, FALSE);
	  *((char*)buffer++) = 'f';
	  break;
	case 'e':
	  buffer += formatDouble(buffer, va_arg(list, double), 10, FALSE, TRUE);
	  break;
	case 'E':
	  buffer += formatDouble(buffer, va_arg(list, double), 10, TRUE, TRUE);
	  break;
	case 'g': //print shorter scientific number (e.g. 392.65 or 9.824e13)
	  break;
	case 'G': //print shorter scientific number (e.g. 392.65 or 9.824E13)
	  break;
	case 'p': //print pointer address (e.g. B800:0000)
	  break;
	case 'i':
	  buffer += formatInt(buffer, va_arg(list, unsigned int), 10, TRUE, FALSE);
	  break;
	case 'u':
	  buffer += formatInt(buffer, va_arg(list, unsigned int), 10, FALSE, FALSE);
	  break;
	case 'x':
	  *((char*)buffer++) = '0';
	  *((char*)buffer++) = 'x';
	  buffer += formatInt(buffer, va_arg(list, unsigned int), 16, FALSE, FALSE);
	  break;
	case 'X':
	  *((char*)buffer++) = '0';
	  *((char*)buffer++) = 'X';
	  buffer += formatInt(buffer, va_arg(list, unsigned int), 16, FALSE, TRUE);
	  break;
	case 'o':
	  buffer += formatInt(buffer, va_arg(list, unsigned int),  8, FALSE, TRUE);
	  break;
	case 'd':
	  buffer += formatDouble(buffer, va_arg(list, double), 10, FALSE, FALSE);
	  break;
	case 'c':
	  *((unsigned char*)buffer++) = (unsigned char)va_arg(list, unsigned int);
	  break;
// 	case 's':
//           char* str = va_arg(list, char*);
//           int len = strlen(str);
// 	  memcpy( buffer, str, strlen(str) );
//           buffer += len;
// 	  break;
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

// void fprintf(void *buffer, const char *format, ...)
// {
//   va_list list;
//   va_start(list, format);
//   formatBuf(buffer, format, list/* <- Don't know for sure if this works*/);
// }
// 
// void printf(const char *format, ...)
// {
//   va_list list;
//   va_start(list, format);
//   ttyPtr -= (int)(ttyBuf + ttyPtr) - (int)formatBuf(ttyBuf + ttyPtr, format, list/* <- Don't know for sure if this works*/);
// }
