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

#ifndef __FS_STREAM_H
#define __FS_STREAM_H

#define DEFAULT_STREAM_SIZE 0x1000

#define EOF             0x19

#define TYPE_DTND       0x001
#define TYPE_LINK       0x002
#define TYPE_CHAR       0x003
#define TYPE_BLCK       0x004
#define TYPE_PIPE       0x005
#define TYPE_SOCK       0x006

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

struct __STREAM_DATA
{
  uint32_t size;
  struct _STREAM_NODE *data;
  struct __STREAM *next_stream;
  struct __STREAM *prev_stream;
  struct __STREAM *stream;
};

struct __STREAM
{
  uint32_t start_cursor_low;
  uint32_t start_cursor_hi;
  uint32_t end_cursor_low;
  uint32_t end_cursor_hi;
  uint32_t stream_type;
  struct __STREAM_DATA *data;
};

typedef struct __STREAM stream;

stream* stream_open();
void stream_close(stream *s);
void stream_write(stream *s, char *data);
char* stream_read(stream *s, char* buf, size_t num);
void stream_seek(stream *s, int offset, enum seektype origin);
#endif