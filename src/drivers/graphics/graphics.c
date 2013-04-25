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

/**
 * This function is used to copy an image buffer somewhere into an other image buffer.
 *
 * @param src
 *   The source image buffer.
 *
 * @param dest
 *   The destination image buffer.
 *
 * @param x
 *   The x coordinate in destination image of the left-top pixel of the source image.
 *
 * @param y
 *   The y coordinate in destination image of the left-top pixel of the source image.
 */
void imageBufferCpy(imageBuffer src, imageBuffer dest, int x, int y)
{
  unsigned int i       = 0,
               maxX    = (dest.width>src.width+x)?src.width:dest.width-x,
               maxY    = (dest.height>src.height+y)?src.height:dest.height-y,
               crsPix  = (unsigned int)src.buffer,
               destPix = (unsigned int)dest.buffer+x+dest.width*y;
  for(;i<maxY;i++)
  {
    memcpy((char*)destPix,(char*)crsPix,maxX);
    crsPix  += src.width;
    destPix += dest.width;
  }
}

/**
 * This function is used to copy a part of an image buffer somewhere into an other image buffer.
 *
 * @param src
 *   The source image buffer.
 *
 * @param dest
 *   The destination image buffer.
 *
 * @param x
 *   The x coordinate in destination image of the left-top pixel of the source image.
 *
 * @param y
 *   The y coordinate in destination image of the left-top pixel of the source image.
 *
 * @param width
 *   The width of the source image that should be copied.
 *
 * @param heigth
 *   The heigth of the source image that should be copied.
 *
 * @param partx
 *   The x coordinate of the part from src that should be copied.
 *
 * @param party
 *   The y coordinate of the part from src that should be copied.
 */
void imageBufferPartCpy(imageBuffer src, imageBuffer dest, int x, int y, int width, int height, int partx, int party)
{
  int i       = 0,
      maxX    = (src.width>width+partx)?width:src.width-partx,
      maxY    = (src.height>height+party)?height:src.height-party,
      crsPix  = (int)src.buffer + (src.width * party) + partx,
      destPix = (int)dest.buffer + x + dest.width*y;
      maxX    = (dest.width>maxX+x)?maxX:dest.width-x;
      maxY    = (dest.height>maxY+y)?maxY:dest.height-y;
  for(;i<maxY;i++)
  {
    memcpy((void*)destPix,(void*)crsPix,maxX);
    crsPix  += src.width;
    destPix += dest.width;
  }
}

// In future there will be scale and rotate functions here to.

/**
 * This function is used to print an image buffer to the screen.
 *
 * @param src
 *   The source image buffer.
 *
 * @param x
 *   The x coordinate in destination image of the left-top pixel of the source image.
 *
 * @param y
 *   The y coordinate in destination image of the left-top pixel of the source image.
 */
void drawBuffer(imageBuffer src, int x, int y)
{
  imageBufferCpy(src, getScreenBuf(), x, y);
}

/**
 * This function is used to print a part of an image buffer to the screen.
 *
 * @param src
 *   The source image buffer.
 *
 * @param x
 *   The x coordinate in destination image of the left-top pixel of the source image.
 *
 * @param y
 *   The y coordinate in destination image of the left-top pixel of the source image.
 *
 * @param width
 *   The width of the source image that should be copied.
 *
 * @param heigth
 *   The heigth of the source image that should be copied.
 *
 * @param partx
 *   The x coordinate of the part from src that should be copied.
 *
 * @param party
 *   The y coordinate of the part from src that should be copied.
 */
void drawBufferPart(imageBuffer src, int x, int y, int width, int height, int partx, int party)
{
  imageBufferPartCpy(src, getScreenBuf(), x, y, width, height, partx, party);
}

/**
 *
 */
imageBuffer newImageBuffer(int width, int height)
{
  imageBuffer img = (imageBuffer){kmalloc(width*height*getScreenDepth()),width,height};
  memset(img.buffer,0,width*height*getScreenDepth());
  return img;
}
