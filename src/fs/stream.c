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
*stream_append_node(struct __STREAM_DATA *s, size_t size)
{
  struct __STREAM_NODE *carriage = s->data;
  while (carriage->next_node != NULL)
    carriage = carriage->next_node;

  struct __STREAM_NODE *tmp = kalloc(sizeof(struct __STREAM_NODE));
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
struct __STREAM_NODE
*stream_find_node(struct __STREAM_DATA *s, size_t offset)
{
  if (s == NULL)
    return NULL;
  struct __STREAM_NODE *carriage = s->data;
  while (carriage->segment_offset + carriage->segment_size < offset)
  {
    carriage = carriage->next_node;
    if (carriage == NULL)
      return NULL;
  }
  return carriage;
}

stream* stream_open()
{
  stream* s = kalloc(sizeof(stream));
  if (s == NULL)
    goto no_clean_up;
  memset(s, 0, sizeof(stream));

  s->data = kalloc(sizeof(struct __STREAM_DATA));
  if (s->data == NULL)
    goto stream_clean_up;
  memset(s->data, 0, sizeof(struct __STREAM_DATA));

  s->data->size = DEFAULT_STREAM_SIZE;
  s->data->data = kalloc(sizeof(struct __STREAM_NODE));
  stream_init_node(s->data->data, DEFAULT_STREAM_SIZE);
  

  return s;

data_clean_up:
  free(s->data);
stream_clean_up:
  free(s);
no_clean_up:
  return NULL;
}
