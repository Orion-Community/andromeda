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

#include "Include/vga.h"
#include "Include/graphics.h"
#include "Include/text.h" 

  /**
   * This is the initial function for the VGA driver
   * 
   * @return
   *   false if failt, true if succeded
   */
boolean vgaInit()
{
  /**
   * What is should do:
   *   1) load a system preference file (or use an already loaded one) to get prefered video mode.
   *   2) setVideoMode( prefered video mode )
   *     If (return = -1): setVideoMode( 0 ) 
   *       If (return = -1): return false
   *   3) textInit()
   *     If (return = false): return false
   *   4) return true;
   */
  return false;
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
  return -1;
}


