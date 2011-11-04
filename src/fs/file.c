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

struct _STREAM_NODE *stream_init_node(struct _STREAM_NODE* s, size_t size)
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

stream* stream_open()
{
  stream *s = kalloc(sizeof(stream));
  if (s == NULL)
    return NULL;

  s->size = DEFAULT_STREAM_SIZE;
  s->cursor = 0;
  s->path = NULL;
  s->rights = 0x2FF; // Grant ALL rights!

  s->data = stream_init_node(kalloc(sizeof(struct _STREAM_NODE)),
                                                           DEFAULT_STREAM_SIZE);
  if (s->data == NULL)
  {
    free(s);
    return NULL;
  }
  return s;
}