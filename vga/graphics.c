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
unsigned long long int charData[2] = {
         4359034079331909180,// = 0x3c7e666666667e3c
         1745171347368581375 // = 0x18381818181818ff
}

void graphicsInit(unsigned int width, unsigned int heigth, unsigned int depth)
{
  screenWidth = width;
  screenHeigth = heigth;
  screenColorDepth = depth;
  screenbuf = kalloc(screenWidth*screenHeigth*screenColorDepth);
}

void graphicsInit()
{
#ifdef HD
  /*
   * Note that the code isn't HD ready jet. Only low res. VGA is allowed.
   */
  graphics_init(???,???,???);
#else
  graphics_init(320,200,1);
#endif
}

void putPixel(unsigned int x, unsigned int y, unsigned int color)
{
  memcpy((unsigned int*)color,screenbuf+(x+y*screenWidth)*screenColorDepth,screenColorDepth)
}

unsigned int getPixel(unsigned int x, unsigned int y)
{
  return *((unsigned int *)(screenbuf+(x+y*screenWidth)*screenColorDepth));
}

void drawBuf(unsigned int x, unsigned int y,unsigned int heigth, unsigned int width, void* buffer)
{
  if( ((x+width)>screenWidth) | ((y+heigth)>screenHeigth) ) return;
  int i=0,i2=(x+y*screenWidth)*screenColorDepth;

  while(i<y)
  {
    memcpy(buffer+(i*width),screenbuf+i2,width);
    i++;
    i2+=screenWidth;
  }
}

void drawBuf(unsigned int x, unsigned int y,unsigned int heigth, unsigned int width,unsigned int bufHeigth, unsigned int bufWidth, void* buffer)
{
  if( ((x+width)>screenWidth) | ((y+heigth)>screenHeigth) ) return;
  int ix,iy = 0;
#ifdef HD
  char* buf = kalloc(width*heigth*screenColorDepth);
  memset(buf,0x00,width*heigth*screenColorDepth);
  int i;
  float xScale = width/bufWidth,yScale = heigth/bufHeigth;
  float newX,newY;
  for(;iy<bufHeigth;iy++)
  {
    newY = iy*yScale;
    for(ix=0;ix<bufWidth;ix++)
    {
      newX = ix*xScale;
      if (ix*xScale = (int)newX)
      {
        if (iy*yScale = (int)newY)
        {
          memcpy(buffer+((newY*width+newX)*screenColorDepth),buf+((newY*width+newX)*screenColorDepth),screenColorDepth);
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
          for(i=0;i<screenColorDepth;i++)
          {
            *(buf+i+((((int)newY)*width+((int)newX))*screenColorDepth))) += (char)((*(buffer+i+((((int)newY)*width+((int)newX))*screenColorDepth)))*(((int)newX)-newX)*(((int)newY)-newY));
          }
          for(i=0;i<screenColorDepth;i++)
          {
            *(buf+((((int)newY + 1)*width+((int)newX))*screenColorDepth))) += (char)((*(buffer+((((int)newY)*width+((int)newX))*screenColorDepth)))*(((int)newX)-newX)*(newY-((int)newY + 1)));
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
  for(;iy<Heigth;iy++)
  {
    for(ix=0;ix<Width;ix++)
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
  unsigned long long chr = charData[character];
  int i = 0;
  for(;i<64;i++)
  {
    buf[i*3]   = ( (chr << i) & 0x0000000000000001 ) * 0xff;
    buf[i*3+1] = ( (chr << i) & 0x0000000000000001 ) * 0xff;
    buf[i*3+2] = ( (chr << i) & 0x0000000000000001 ) * 0xff;
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
