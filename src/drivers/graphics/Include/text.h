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

#ifndef __TEXT_H
#define __TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

bool textInitG();
void printCharToBuffer(imageBuffer buffer, char chr, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color);
void printStringToBuffer(imageBuffer buffer, char* str, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color);
void drawChar(unsigned int x, unsigned int y, char chr, unsigned int bgcolor, unsigned int color);
void drawString(char* str, unsigned int x, unsigned int y, unsigned int bgcolor, unsigned int color);

#ifdef __cplusplus
}
#endif

#endif
