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

#include <fs/file.h>
#include <stdlib.h>

file_t *file_open(char *path)
{
  if (path != NULL)
  {
#ifdef STREAM_DBG
    printf("WARNING: Paths not supported yet!\n");
#endif
    return NULL;
  }

  file_t *f = kalloc(sizeof(file_t));
  if (f == NULL)
    goto clean_up;
  stream_t *s = kalloc(sizeof(stream_t));
  if (s == NULL)
    goto clean_file;

  memset(s, 0, sizeof(stream_t));

  stream_t *s2 = stream_init(s, DEFAULT_STREAM_SIZE, 0);
  if (s2 == NULL)
    goto clean_stream;

  return f;

clean_stream:
  free(s);
clean_file:
  free(f);
clean_up:
  return NULL;
}

int
file_read(file_t *stream, char *buffer, size_t buffer_size)
{
  return -E_SUCCESS;
}