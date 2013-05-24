/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#ifndef VGA_H
#define VGA_H

#ifdef __cplusplus
extern "C" {
#endif

// #define KEYBUF 0xB8000
#define KEYBUF 0xC00B8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define OL_WHITE_TXT 0x07

struct curPos
{
	uint32_t x;
	uint32_t y;
//        uint16_t *vidmem;
	int tabwidth;
};

#ifdef __cplusplus
}
#endif

#endif
