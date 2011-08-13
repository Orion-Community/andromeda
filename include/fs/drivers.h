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

#ifndef __FS_DRIVERS_H
#define __FS_DRIVERS_H

#include <fs/file.h>

struct _FS_DRIVER
{
  unsigned int id;
  char name[8];
  FILE* (*open) (char* name, unsigned int rights);
  int (*close)  (FILE* fd);
  int (*read)   (FILE* fd, char* buf, size_t num);
  int (*write)  (FILE* fd, char* buf, size_t num);
  struct _FS_DRIVER *next;
};

#endif