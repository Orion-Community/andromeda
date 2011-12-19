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

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

struct imageBuffer_s
{
  char*        buffer;
  unsigned int width;
  unsigned int height;
};
typedef struct imageBuffer_s imageBuffer;

void imageBufferCpy(imageBuffer src, imageBuffer dest, int x, int y);
void imageBufferPartCpy(imageBuffer src, imageBuffer dest, int x, int y, int width, int height, int partx, int party);
void drawBuffer(imageBuffer src, int x, int y);
void drawBufferPart(imageBuffer src, int x, int y, int width, int height, int partx, int party);
imageBuffer newImageBuffer(int width, int height);

#ifdef __cplusplus
}
#endif

#endif
