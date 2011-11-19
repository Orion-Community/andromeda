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
 * stream_init_node returns what should be the first node for the stream
 */
struct __STREAM_NODE
*stream_init_node(struct __STREAM_NODE *s, size_t size)
{
  if (s == NULL || size == 0)
    return NULL;
  s->base = kalloc(size*SECTOR_SIZE);
  if (s->base == NULL)
    return NULL;
  s->segment_base = 0;
  s->segment_size = size*SECTOR_SIZE;

  char* buffer = (char*)s->base;
  buffer[0] = EOF;
  return s;
}

/**
 * stream_close_node closes an individual node and returns the pointer to the
 * next node. The offsets aren't updated!!!
 */
struct __STREAM_NODE
*stream_close_node(struct __STREAM_NODE *s)
{
  struct __STREAM_NODE *ret = s->next_node;
  free(s);
  return ret;
}

/**
 * stream_append_node appends a node to the end of the list so the size of the
 * stream can be growed.
 */
struct __STREAM_NODE
*stream_append_node(stream *s, size_t size)
{
  struct __STREAM_NODE *carriage = s->data;
  while (carriage->next_node != NULL)
    carriage = carriage->next_node;

  struct __STREAM_NODE *tmp = kalloc(sizeof(struct __STREAM_NODE));
  if (tmp == NULL)
    return NULL;

  tmp->segment_base = carriage->segment_base + carriage->segment_size;

  tmp->segment_size = size;
  tmp->base = kalloc(size*SECTOR_SIZE);
  if (tmp->base == NULL)
  {
    free(tmp);
    return NULL;
  }
  memset(tmp->base, 0, size);


  carriage->next_node = tmp;
  return tmp;
}

/**
 * stream_find_node finds the node to go with the requested location.
 */
struct __STREAM_NODE
*stream_find_node(stream *s, size_t offset)
{
  if (s == NULL)
    return NULL;
  struct __STREAM_NODE *carriage = s->data;
  while (carriage->segment_base + carriage->segment_size < offset)
  {
    carriage = carriage->next_node;
    if (carriage == NULL)
      return NULL;
  }
  return carriage;
}


stream
*stream_init(size_t stream_size, uint64_t offset)
{
  if (stream_size == 0)
    return NULL;
  
  return NULL;
}