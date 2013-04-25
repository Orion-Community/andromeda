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

#include <stdio.h>
#include "Include/VGA.h"
#include "Include/graphics.h"
#include "Include/text.h"

char* font;

  /**
   * This function loads the basinc fonts.
   */
bool textInitG() // This will be named textInit again after I implemented the text mode driver too.
{
  /**
   * @TODO:
   *   - load atleast 1 font image and place them in font.
   */
  font = kmalloc(256);
  memset(font,0,256);
  return true;
}

imageBuffer getCharBuffer(char chr, unsigned int bgcolor, unsigned int color)
{
  imageBuffer buffer = {kmalloc(64*getScreenDepth()),8,8};
  char* buf = buffer.buffer;
  int i = 0;
  for (;i<64;i++)
  {
    memset(buf,font[chr]?color:bgcolor,getScreenDepth());
    buf += getScreenDepth();
  }
  return buffer;
}

  /**
   * This function prints a character to an image buffer.
   *
   * @param buffer
   *   The buffer the string should be printed to.
   *
   * @param chr
   *   The character that should printed to the image buffer.
   *
   * @param bgcolor
   *   The background color for the character.
   *
   * @param color
   *   The character color.
   *
   * @param x
   *   The x coordinate in image buffer for the character.
   *
   * @param y
   *   The y coordinate in image buffer for the character.
   */
void printCharToBuffer(imageBuffer buffer, char chr, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color)
{
  imageBuffer buf = getCharBuffer(chr,bgcolor,color);
  imageBufferCpy(buf,buffer,x,y);
  free(buf.buffer);
}

  /**
   * This function prints a sting to an image buffer.
   *
   * @param buffer
   *   The buffer the string should be printed to.
   *
   * @param str
   *   The string that should printed to the image buffer.
   *
   * @param bgcolor
   *   The background color for the characters.
   *
   * @param color
   *   The character color.
   *
   * @param x
   *   The x coordinate in image buffer for the character.
   *
   * @param y
   *   The y coordinate in image buffer for the character.
   */
void printStringToBuffer(imageBuffer buffer, char* str, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color)
{
  int i       = 0,
      i2      = 0,
      maxChrs = (int)((buffer.width-x)/8),
      len     = strlen(str),
      strToDo = 0;

  while ( i < len )
  {
    if ( len - i > maxChrs )
    {
      strToDo = maxChrs;
      while ( strToDo > 0 )
      {
        if ( str[strToDo] == ' ' )
        {
          str[strToDo] == '\n';
          break;
        }
        strToDo--;
      }
      if ( strToDo == 0 )
        strToDo = maxChrs;
    } else
      strToDo = 0;
    for (i2 = 0; i2 < strToDo; i2++)
    {
      if ( str[i]=='\n' )
      {
        i++;
        break;
      }
      printCharToBuffer(buffer,str[i],x+i2*8,y,bgcolor,color);
      i++;
    }
    y += 8;
    if ( y > buffer.height )
      break;
  }
}

  /**
   * This function draws a character to the screen.
   *
   * @param x
   *   The x coordinate on the screen for the top-left pixel of the character.
   *
   * @param c
   *   The y coordinate on the screen for the top-left pixel of the character.
   *
   * @param chr
   *   The character that should draw to the screen.
   *
   * @param bgcolor
   *   The background color for the character.
   *
   * @param color
   *   The character color.
   */
void drawChar(unsigned int x, unsigned int y, char chr, unsigned int bgcolor, unsigned int color)
{
  drawBuffer(getCharBuffer(chr,bgcolor,color),x,y);
}


  /**
   * This function draws a string to the screen.
   *
   * @param x
   *   The x coordinate on the screen for the top-left pixel of the string.
   *
   * @param c
   *   The y coordinate on the screen for the top-left pixel of the string.
   *
   * @param str
   *   The string that should be draw to the screen.
   *
   * @param bgcolor
   *   The background color for the characters.
   *
   * @param color
   *   The character color.
   */
void drawString(char* str, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color)
{
  printStringToBuffer(getScreenBuf(),str,x,y,bgcolor,color);
}
