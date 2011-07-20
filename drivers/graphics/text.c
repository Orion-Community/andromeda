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

#include "Include/graphics.h"
#include "Include/text.h" 

  /**
   * This function loads the basinc fonts.
   */
boolean textInit()
{
  /**
   * load atleast 1 font image and convert them into an array of buffers.
   */
  return false;
}

  /**
   * This function returns the image buffer of a single character.
   * 
   * @param chr
   *   The character that should be view by the image buffer.
   */
imageBuffer char2ImageBuffer(char chr) //Don't know for sure if I will keep this function
{
  /**
   * 
   */
  return ...;
}

  /**
   * This function returns the image buffer of a string.
   * 
   * @param str
   *   The string that should be view by the image buffer.
   */
imageBuffer string2imageBuffer(char* str) //Don't know for sure if I will keep this function
{
  /**
   * 
   */
  return ...;
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
   */
void drawChar(int x, int y, char chr)
{
  /**
   * 
   */
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
   */
void drawString(int x, int y, char* str)
{
  /**
   * 
   */
}
