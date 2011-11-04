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

#define DEFAULT_STREAM_SIZE 0x1000

#define OWNER_READ      0x001
#define OWNER_WRITE     0x002
#define OWNER_EXECUTE   0x004

#define GROUP_READ      0x008
#define GROUP_WRITE     0x010
#define GROUP_EXECUTE   0x020

#define ALL_READ        0x040
#define ALL_WRITE       0X080
#define ALL_EXECUTE     0x100

#define DIRECTORY_DATA  0x200

#define EOF             0x19

enum seektype {SEEK_SET, SEEK_CUR, SEEK_END};

struct _STREAM_NODE
{
  void *base;
  void *end;
  uint32_t segment_offset;
  uint32_t segment_size;
  struct _STREAM_NODE *prev_node;
  struct _STREAM_NODE *next_node;
};

struct _STREAM
{
  uint32_t size;
  uint32_t cursor;
  char *path;
  uint16_t rights;
  struct _STREAM_NODE *data;
  
};

typedef struct _STREAM stream;

#endif