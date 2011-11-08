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

#include <stdlib.h>
#include <fs/stream.h>

/**
 * stream_init_inode returns what should be the first node for the stream
 */
struct _STREAM_NODE *stream_init_node(struct _STREAM_NODE *s, size_t size)
{
  if (s == NULL)
    return NULL;
  else if (size == 0)
  {
    s->base = NULL;
    s->end = NULL;
    s->segment_offset = 0;
    s->segment_size = 0;
    return s;
  }
  s->base = kalloc(size);
  if (s->base == NULL)
    return NULL;
  s->end = (void*)((addr_t)s->base+size);
  s->segment_offset = 0;
  s->segment_size = size;

  char* buffer = (char*)s->base;
  buffer[0] = EOF;
  return s;
}

/**
 * stream_close_node closes an individual node and returns the pointer to the
 * next node. The offsets aren't updated!!!
 */
struct _STREAM_NODE *stream_close_node(struct _STREAM_NODE *s)
{
  struct _STREAM_NODE *ret = s->next_node;
  free(s);
  return ret;
}

/**
 * stream_append_node appends a node to the end of the list so the size of the
 * stream can be growed.
 */
struct _STREAM_NODE *stream_append_node(stream *s, size_t size)
{
  struct _STREAM_NODE *carriage = s->data;
  while (carriage->next_node != NULL)
    carriage = carriage->next_node;

  struct _STREAM_NODE *tmp = kalloc(sizeof(struct _STREAM_NODE));
  if (tmp == NULL)
    return NULL;

  tmp->segment_offset = carriage->segment_offset + carriage->segment_size;
  tmp->segment_size = size;
  tmp->base = kalloc(size);
  if (tmp->base == NULL)
  {
    free(tmp);
    return NULL;
  }
  tmp->end = (void*)((addr_t)tmp->base + size);
  memset(tmp->base, 0, size);

  carriage->next_node = tmp;
  return tmp;
}

/**
 * stream_find_node finds the node to go with the cursor location.
 */
struct _STREAM_NODE *stream_find_node(stream *s, size_t offset)
{
  if (s == NULL)
    return NULL;
  struct _STREAM_NODE *carriage = s->data;
  while (carriage->segment_offset + carriage->segment_size < offset)
  {
    carriage = carriage->next_node;
    if (carriage == NULL)
      return NULL;
  }
  return carriage;
}

/**
 * stream_open opens a new stream.
 */
stream* stream_open()
{
  stream *s = kalloc(sizeof(stream));
  if (s == NULL)
    return NULL;

  s->size = DEFAULT_STREAM_SIZE;
  s->cursor = 0;
  s->path = NULL;
  s->rights = 0x1FF; // Grant ALL rights!

  s->data = stream_init_node(kalloc(sizeof(struct _STREAM_NODE)),
                                                           DEFAULT_STREAM_SIZE);
  if (s->data == NULL)
  {
    free(s);
    return NULL;
  }
  return s;
}

/**
 * Close the stream
 */
void stream_close(stream *s)
{
  while (TRUE)
  {
    s->data = stream_close_node(s->data);
    if (s->data == NULL)
      break;
  }
  free(s);
}

/**
 * stream_write writes to the cursor location in the stream.
 * The cursor is moved afterwards.
 */
void stream_write(stream *s, char *data)
{
  if (s == NULL || data == NULL)
    return;

  struct _STREAM_NODE *node = stream_find_node(s, s->cursor);
  if (node == NULL)
    return;
  char *buffer = (char*)node->base;
  size_t node_idx = 0;
  size_t idx = 0;
  boolean endoffile = false;
  for (; idx < strlen(data); node_idx++, idx ++)
  {
    if (node_idx >= node->segment_size)
    {
      if (node->next_node == NULL)
        node = stream_append_node(s, strlen(data) - idx);
      else
        node = node->next_node;
      if (node == NULL)
        return;
      buffer = (char*)node->base;
      node_idx = 0;
    }
    if (buffer[node_idx] == EOF)
      endoffile = true;
    buffer[node_idx] = data[idx];
  }
  if (node_idx < node->segment_size)
    buffer[node_idx++] = EOF;
  else
  {
    node = stream_append_node(s, DEFAULT_STREAM_SIZE);
    buffer = (char*)node->base;
    buffer[0] = EOF;
  }
  s->cursor += strlen(data);
  if (s->cursor > s->size)
    s->size = s->cursor;
}

/**
 * stream_read reads a number of characters from the cursor position in the
 * stream. The cursor is moved afterwards.
 */
char* stream_read(stream *s, char* buf, size_t num)
{
  if (s == NULL || num == 0 || buf == NULL)
    return NULL;
  
  char* buffer = NULL;
  memset(buf, 0, num);

  size_t idx = 0;
  size_t node_idx;
  struct _STREAM_NODE *node = stream_find_node(s, s->cursor);
  if (node == NULL)
    return NULL;
  node_idx = s->cursor - node->segment_offset;
  buffer = (char*)node->base;

  for (; idx < num; idx++, node_idx++)
  {
    if (node_idx >= node->segment_size)
    {
      node = stream_find_node(s, s->cursor + idx);
      if (node == NULL)
      {
        return NULL;
      }
      node_idx = (s->cursor + idx) - node->segment_offset;
    }
    if (buffer[node_idx] == EOF)
      break;
    buf[idx] = buffer[node_idx];
  }
  s->cursor += idx;

  return buf;
}

/**
 * stream_seek moves the cursor to the requested location in the stream.
 */
void stream_seek(stream *s, int offset, enum seektype origin)
{
  switch (origin)
  {
    case SEEK_SET:
      if (offset > s->size || offset < 0)
        return;
      s->cursor = offset;
      break;
    case SEEK_CUR:
      if ((offset > (s->size - s->cursor)) || (-offset > s->cursor))
        return;
      s->cursor += offset;
      break;
    case SEEK_END:
      if (-offset > s->size)
        return;
      s->cursor = s->size + offset;
      break;
    default:
      return;
      break;
  }
}