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
#include "include/TTY.h"

void ttyInit()
{
  //tyPtr = 0;
  //ttyFirstChar = 0;
  //ttyBuf = kalloc(TTYBUFSIZE);
  //memset(ttyBuf,0x00,TTYBUFSIZE);

  /*
   * TODO: Bind drawTTY() on timer.
   */

}

void drawTTY()
{
  drawString(0,0,ttyViewLine);
  //unsigned int y = -(int)(ttyPtr / -40);
  //drawChar((unsigned int)ttyPtr-y,y,'_');
  /*
   * TODO: draw cursor.
   */
}