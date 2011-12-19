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

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_STREAM_SIZE 0x4 /* Defined as multiple of SECTOR_SIZE */
#define SECTOR_SIZE 0x100

#define EOF             0x19

#define TYPE_CMMN       0x001 /* Common file type */
#define TYPE_DIR        0x002 /* Directory file type */
#define TYPE_LINK       0x003 /* Link file type */
#define TYPE_CHAR       0x004 /* Character device file type */
#define TYPE_BLCK       0x005 /* Block device file type */
#define TYPE_PIPE       0x006 /* Pipe file type */
#define TYPE_SOCK       0x007 /* Socket file type */

/**
 * Stream node is a segment in the actual stream.
 * base         The base address of the data
 * segment_base The offset into the stream (in bytes)
 * segment_size The size of the segment (in bytes)
 * prev_node    The pointer to the previous segment
 * next_node    The pointer to the next segment
 */

struct __STREAM_NODE
{
  void *base; /* Base address of stream data */
  uint64_t segment_base; /* Base index in number of sectors */
  size_t segment_size; /* size of segment in multiple of SECTOR_SIZE */
  struct __STREAM_NODE *prev_node; /* Pointer to the previous block */
  struct __STREAM_NODE *next_node; /* Pointer to the next block */
};

struct __STREAM
{
  struct __STREAM_NODE *data; /* Pointer to data */

  uint64_t size; /* size of full stream */
  uint64_t buffer_index; /* start of buffered part of stream */

  size_t buffer_size; /* size of buffered part of stream */
};

typedef struct __STREAM stream_t;

stream_t *stream_init(stream_t *s, size_t stream_size, uint64_t offset);
int stream_close(stream_t *stream);
size_t stream_read(stream_t *stream, uint64_t cursor, size_t length, void *b);
size_t stream_write                          (stream_t *stream, uint64_t cursor,
                                      size_t length, void *b, uint32_t *growth);

#ifdef __cplusplus
}
#endif

#endif