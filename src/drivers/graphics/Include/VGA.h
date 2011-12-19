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


#include "graphics.h"

#ifndef __VGA_H
#define __VGA_H

#ifdef __cplusplus
extern "C" {
#endif

struct pixel_s
{
  unsigned int x;
  unsigned int y;
};

typedef struct pixel_s pixel;

bool vgaInit();
int setVideoMode(int mode);
int setModeViaPorts(int width, int height,int chain4);
void updateScreen();
imageBuffer getScreenBuf();
inline unsigned int getScreenWidth();
inline unsigned int getScreenHeight();
inline unsigned int getScreenDepth();

#ifdef __cplusplus
}
#endif

#endif
