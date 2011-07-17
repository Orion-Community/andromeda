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

#include <tty/tty.h>
#include <stdio.h>
#include <stdlib.h>
#include <error/panic.h>

#define TERMINALS 12

tty_t terminals[TERMINALS];

void tty_init()
{
  int i = 0;
  for (; i < TERMINALS; i++)
  {
    terminals[i].io = nalloc(sizeof(tty_io_t));
    if (terminals[i].io == NULL)
    {
      panic("Out of memory in tty_init");
    }
    terminals[i].io -> input = nalloc(sizeof(buffer_s));
    terminals[i].io -> output = nalloc(sizeof(buffer_s));
    if (terminals[i].io -> input == NULL || terminals[i].io -> output == NULL)
    {
      panic("Out of memory in tty_init");
    }
  }
}