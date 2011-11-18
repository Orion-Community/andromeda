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

#define TYPE_CMMN       0x001 /* Common file type */
#define TYPE_DIR        0x002 /* Directory file type */
#define TYPE_LINK       0x003 /* Link file type */
#define TYPE_CHAR       0x004 /* Character device file type */
#define TYPE_BLCK       0x005 /* Block device file type */
#define TYPE_PIPE       0x006 /* Pipe file type */
#define TYPE_SOCK       0x007 /* Socket file type */

enum seektype {SEEK_SET, SEEK_CUR, SEEK_END};

/**
 * Needs a bit of rethinking, now this implements a random fragment size
 * Might be more efficient to stick to disk block sizes
 */

struct __STREAM_NODE
{
  void *base; /* Base address of stream data */
  uint64_t segment_base; /*  */
  size_t segment_size;
  struct __STREAM_NODE *prev_node;
  struct __STREAM_NODE *next_node;
};

struct __STREAM
{
  struct __STREAM_NODE *data; /* Pointer to data */

  uint64_t size; /* size of stream */
  uint64_t buffer_index; /* start of buffered part of stream */

  size_t buffer_size; /* size of buffered part of stream */
};

typedef struct __STREAM stream;

stream *stream_init(size_t stream_size, uint64_t offset);
#endif