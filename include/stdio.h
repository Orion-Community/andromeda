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

#ifndef __STDIO_H
#define __STDIO_H

#include <stdlib.h>

#define STDERR 0
#define STDOUT 1
#define STDIN  2

struct buffer_s
{
  char *buffer;
  unsigned int size;
  unsigned int cursor;
  struct buffer_s *next;
  boolean full;
};

typedef struct buffer_s buffer_t;

#endif