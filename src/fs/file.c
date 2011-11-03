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

stream *stream_open(char *path, char *rights)
{
  if (path == NULL)
    return NULL;

  stream *s = kalloc(sizeof(stream));
  memset(s, 0, sizeof(stream));

  s->base = kalloc(DEFAULT_STREAM_SIZE);
  s->end = (void*)((addr_t)s->base + DEFAULT_STREAM_SIZE);
  s->cursor = s->base;
  s->next_node = NULL;
  s->prev_node = NULL;

  return s;
}