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
#include "include/graphics.h"

/*
 * Hardcode font. currently only contains fonts for '0' and '1'.
 */


const unsigned int charData[4] = {
         0x3c7e6666,0x66667e3c, // 0
         0x18381818,0x181818ff  // 1
};

void graphicsInit(unsigned int width, unsigned int heigth, unsigned int depth)
{
  screenWidth = width;
  screenHeight = heigth;
  screenColorDepth = depth;
  screenbuf = kalloc(screenWidth*screenHeight*screenColorDepth);
}

void graphicsSetScreen(unsigned int width, unsigned int heigth, unsigned int depth)
{
  free(screenbuf); // There is no kfree() jet!
  graphicsInit(width,heigth,depth);
}

// Copy color into the x and y coordinate
void putPixel(unsigned int x, unsigned int y, unsigned int color)
{  
  memcpy(
    (unsigned int*)color, //is this correct?
    screenbuf + (x + y * screenWidth) * screenColorDepth, // base addres + ( pixel num. * pixel width )
    screenColorDepth
  );
}

// Retreive the pixel value
unsigned int getPixel(unsigned int x, unsigned int y)
{
  unsigned int pixel = (x + y * screenWidth);
  pixel *= screenColorDepth;
  pixel += screenbuf;
  return *((unsigned int *)pixel); // is this correct?
}

void drawBuf(unsigned int x, unsigned int y,unsigned int height, unsigned int width, void* buffer)
{
  if (((x + width) > screenWidth) || ((y+height) > screenHeight))
    return;
  int i  = 0; // counter for loop.
  int i2 = screenbuf + (x + y * screenWidth) * screenColorDepth; // adres of the first pixel to draw to ( = base addres + ( pixel num. * pixel width ) )

  while(i < height) // loops through each horizontal line of the image buffer.
  {
    memcpy(
      buffer + (i * width),
      i2,
      width
    );
    i++;
    i2 += screenWidth * screenColorDepth; // go 1 pixel down
  }
}

void drawBufScale(unsigned int x, unsigned int y, unsigned int heigth, unsigned int width, unsigned int bufHeigth, unsigned int bufWidth, void* buffer)
{
  if(((x + width) > screenWidth) | ((y + heigth) > screenHeight)) return;
  int ix,iy = 0;
#ifdef HD
  char* buf = kalloc(width * heigth * screenColorDepth);
  memset(buf, 0x00, width * heigth * screenColorDepth);
  int i;
  float xScale = width / bufWidth, yScale = heigth / bufHeigth;
  float newX, newY;
  for(;iy < bufHeigth; iy++)
  {
    newY = iy * yScale;
    for(ix = 0; ix < bufWidth; ix++)
    {
      newX = ix * xScale;
      if (ix * xScale = (int)newX)
      {
        if (iy * yScale = (int)newY)
        {
          memcpy(buffer + ((newY * width + newX) * screenColorDepth), buf + ((newY * width + newX) * screenColorDepth), screenColorDepth);
        } else {
          /*
           * Didn't test this. Hopefully it works :S
           * 
           * The idea is to poit out the new position for every pixel in the old buffer. That 
           * position will probebly be in between 4 pixels. The code will calculate how far the
           * calculated position from the nearest 4 pixels is and will add this to its color to get
           * a nicer scale effect.
           * Note this only works when the color is in RGB format, so it will only work in HD mode.
           * 
           */
          for(i = 0; i < screenColorDepth; i++)
          {
            *(buf + i + ((((int)newY) * width + ((int)newX)) * screenColorDepth))) += (char)((*(buffer + i + ((((int)newY) * width + ((int)newX)) * screenColorDepth))) * (((int)newX) - newX) * (((int)newY) - newY));
          }
          for(i = 0; i < screenColorDepth; i++)
          {
            *(buf + ((((int)newY + 1) * width + ((int)newX)) * screenColorDepth))) += (char)((*(buffer + ((((int)newY) * width + ((int)newX)) * screenColorDepth))) * (((int)newX) - newX) * (newY - ((int)newY + 1)));
          }
        }
      } else {
        if (iy*yScale = (int)newY)
        {
          ;
        } else {
          ;
        }
      }
    }
  }
  drawBuf(x,y,width,heigth,buf);
#else
  float xScale = bufWidth/width,yScale = bufHeigth/heigth;
  for(;iy<heigth;iy++)
  {
    for(ix=0;ix<width;ix++)
    {
      memcpy(buffer+((((int)(iy*yScale))*width+((int)(ix*xScale)))*screenColorDepth),screenbuf+(((y+iy)*screenWidth+x+ix)*screenColorDepth),screenColorDepth);
    }
  }
#endif
}

/*
 * currently only 0 and 1 are posible values for character parameter.
 */
void* getCharBuf(char character)
{
  if(character>1) return getCharBuf((char)0); // draws '0' if invalide character is asked.
  char* buf = kalloc(64*screenColorDepth);
  unsigned int chr1 = charData[character*2];
  unsigned int chr2 = charData[character*2+1];
  int i = 0;
  for(;i<32;i++)
  {
    buf[i*3]   = ( (chr1 << i) & 0x00000001 ) * 0xff;
    buf[i*3+1] = ( (chr1 << i) & 0x00000001 ) * 0xff;
    buf[i*3+2] = ( (chr1 << i) & 0x00000001 ) * 0xff;
  }
  for(i=0;i<32;i++)
  {
    buf[32+i*3]   = ( (chr2 << i) & 0x00000001 ) * 0xff;
    buf[32+i*3+1] = ( (chr2 << i) & 0x00000001 ) * 0xff;
    buf[32+i*3+2] = ( (chr2 << i) & 0x00000001 ) * 0xff;
  }
  return buf;
}

void drawChar(unsigned int x, unsigned int y,char chr)
{
  drawBuf(x,y,8,8,getCharBuf(chr));
}

void drawString(unsigned int x, unsigned int y,char* str)
{
  int i = strlen(str);
  for (;i>0;i--)
  {
    drawChar(x+(i*8),y,*(str+i));
  }
}
