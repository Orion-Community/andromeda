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

#define SEEK_END        0
#define SEEK_CURSOR     1
#define SEEK_BEGIN      2

struct _STREAM
{
  void *base;
  void *end;
  void *cursor;
  uint32_t segment_offset;
  uint32_t segment_size;
  struct _STREAM *prev_node;
  struct _STREAM *next_node;
  char *path;
};

typedef struct _STREAM stream;

#endif