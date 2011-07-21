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
#include "Include/vga.h"
#include "Include/graphics.h"
#include "Include/text.h" 


struct videoMode_s
{
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  unsigned int ax;
  unsigned int ah;
  unsigned int bx;
  char* addr;
};

const struct videoMode_s videomodes[2] = {
    {0320,0200,1,0x0000,0x0013,0x0000,0xB8000}, // 320  x 200  x 256
    {1280,1024,1,0x004F,0x0002,0x011B,0xB8000}  // 1280 x 1024 x 16M | 0xB8000 is just a guess
  };
char* screenbuf; // sreen buffer, containing all pixels that should be written to the screen.
int videoMode;         // the current video mode.

/**
 * This is the initial function for the VGA driver
 * 
 * @return
 *   false if failt, true if succeded
 */
boolean vgaInit()
{
  /**
   * @TODO:
   *   - load settings file or get setting from preloaded settings file.
   *   - get videomode from settings file.
   */
  int mode = 0; //should become a user defined videomode,  from a settings file.
  
  screenbuf = kalloc(1);
  
  if setVideoMode( mode ) == -1
    if setVideoMode(0) == -1
      return false;

  if !textInit();
    return false;

  return true;
}

/**
 * This function sets the videomode
 * 
 * @param mode
 *   Number (or ID) of the videomode. This should corsponding with the videoModes array.
 * 
 * @return
 *   If fails, this will be -1, else it will be or just some value (not -1) or a value
 *   returned by interupt.
 */
int setVideoMode(int mode)
{
  /**
   * Here should be some code to make an bios interupt. It should use a function like this:
   *   int ret = someDoInteruptFunction( 10h , videoModes[mode] -> return , videoModes[mode] -> ah , videoModes[mode] -> ax );
   */
  realloc( screenbuf, videoModes[mode]->heigwidth * videoModes[mode]->height * videoModes[mode]->depth );
  videoMode = mode;
  return -1;
}

/**
 * 
 * 
 * 
 */
void updateScreen()
{
  /**
   * @TODO:
   *   - check if this works for all videoModes.
   */
  memcpy(
    screenBuffer->buffer ,
    videoModes[videoMode]->addr ,
    videoModes[videoMode]->width * videoModes[videoMode]->height * videoModes[videoMode]->depth
  );
}

/**
 * Used to get the screen width.
 * 
 * @return
 *   screen width in pixels.
 */
unsigned int getScreenWidth()
{
  return videoModes[videoMode]->width;
}

/**
 * Used to get the screen height.
 * 
 * @return
 *   screen height in pixels.
 */
unsigned int getScreenHeight()
{
  return videoModes[videoMode]->height;
}

/**
 * Used to get the screen buffer as image buffer.
 * 
 * @return
 *   screen height in pixels.
 */
imageBuffer getScreenBuf()
{
  return (imageBuffer){screenBuffer,videoModes[videoMode]->height,videoModes[videoMode]->height};
}
