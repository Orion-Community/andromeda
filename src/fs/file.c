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
#include <fs/file.h>

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
  return s;
}

struct _STREAM_NODE *stream_close_node(struct _STREAM_NODE *s)
{
  struct _STREAM_NODE *ret = s->next_node;
  free(s);
  return ret;
}

struct _STREAM_NODE *stream_find_node(stream *s, size_t offset)
{
  struct _STREAM_NODE *carriage = s->data;
  while (carriage->segment_offset + carriage->segment_size < offset)
  {
    carriage = carriage->next_node;
  }
  return carriage;
}

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

void stream_write(stream *s, char *data)
{
}

char* stream_read(stream *s, size_t num)
{
  if (s == NULL || num == 0)
    return NULL;

  char* ret = kalloc(num);
  char* buffer = NULL;
  if (ret == NULL)
    return NULL;

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
        free (ret);
        return NULL;
      }
      node_idx = (s->cursor + idx) - node->segment_offset;
    }
    ret[idx] = buffer[node_idx];
  }

  return ret;
}

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