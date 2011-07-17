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

#ifndef __TTY_H
#define __TTY_H

#include <stdio.h>

#define TERMINALS 12

typedef struct {
  buffer_t buffers[0x10];
  char* frameBuf;
  unsigned int cursor_x;
  unsigned int cursor_y;
  unsigned int size_x;
  unsigned int size_y;
  unsigned int screenmode;
} tty_t;

extern tty_t terminals[];

void tty_init();

#endif