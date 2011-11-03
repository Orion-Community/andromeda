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

stream *stream_open()
{
  stream *s = kalloc(sizeof(stream));
  memset(s, 0, sizeof(stream));

  s->segment_offset = 0;
  s->segment_size = DEFAULT_STREAM_SIZE;
  s->base = kalloc(DEFAULT_STREAM_SIZE);
  s->end = (void*)((addr_t)s->base + DEFAULT_STREAM_SIZE);
  s->cursor = s->base;
  s->path = NULL;
  s->next_node = NULL;
  s->prev_node = NULL;

  return s;
}

void stream_attach_file(stream* s, char* path)
{
  s->path = path;
  return;
}

stream* stream_find_head(stream* s)
{
  if (s == NULL)
    return NULL;
  stream *carriage = s;
  for (; carriage->prev_node != NULL; carriage = carriage->prev_node);
  carriage->cursor = carriage->base;
  return carriage;
}

stream* stream_find_tail(stream* s)
{
  if (s == NULL)
    return NULL;
  stream *carriage = s;
  for (; carriage->next_node != NULL; carriage = carriage->next_node);
  carriage->cursor = carriage->end;
  return carriage;
}

stream* stream_seek(stream *s, int idx, uint32_t start)
{
  stream *tmp;
  switch(start)
  {
    case SEEK_BEGIN:
      if (idx < 0)
        return NULL;
      tmp = stream_find_head(s);
      break;
    case SEEK_END:
      if (idx > 0)
        return NULL;
      tmp = stream_find_tail(s);
      break;
    case SEEK_CURSOR:
    default:
      printf("WARNING: Unimplemented seek option!\n");
      return NULL;
      break;
  }
  return tmp;
}

void stream_close(stream* s)
{
  if (s == NULL)
    return;
  if (s->prev_node != NULL)
  {
    stream_close(stream_seek(s, 0, SEEK_BEGIN));
  }
  stream *carriage = s;
  stream *tmp = s->next_node;

  for (; carriage->next_node != NULL; carriage = tmp, tmp = tmp->next_node)
  {
    free(carriage);
  }

  return;
}