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

#ifndef __FS_FILE_H
#define __FS_FILE_H

#include <stdlib.h>
#include <fs/stream.h>

#define OWNER_READ      0x001
#define OWNER_WRITE     0x002
#define OWNER_EXECUTE   0x004

#define GROUP_READ      0x008
#define GROUP_WRITE     0x010
#define GROUP_EXECUTE   0x020

#define ALL_READ        0x040
#define ALL_WRITE       0X080
#define ALL_EXECUTE     0x100

struct __FILE
{
  char *path;
  stream *data;
  uint16_t file_rights;
  uint64_t stream_cursor;
};

typedef struct __FILE file;

#endif