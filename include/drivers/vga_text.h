/*
 *  Andromeda
 *  Copyright (C) 2011 - 2015  Bart Kuivenhoven
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

#include <ioctl.h>

#ifndef __DRIVERS_VGA_TEXT_H
#define __DRIVERS_VGA_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

int vga_text_init(struct device* parent);
int vga_text_set_fg_colour(uint16_t colour);
int vga_text_set_bg_colour(uint16_t colour);

#ifdef __cplusplus
}
#endif

#endif
