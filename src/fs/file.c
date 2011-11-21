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

file_t*
file_open(char *path)
{
  if (path != NULL)
  {
#ifdef STREAM_DBG
    printf("WARNING: Paths not supported yet!\n");
#endif
    return NULL;
  }

  file_t *file = kalloc(sizeof(file_t));
  if (file == NULL)
    goto clean_up;
  stream_t *stream = kalloc(sizeof(stream_t));
  if (stream == NULL)
    goto clean_file;

  memset(stream, 0, sizeof(stream_t));

  stream_t *stream2 = stream_init(stream, DEFAULT_STREAM_SIZE, 0);
  if (stream2 == NULL)
    goto clean_stream;

  return file;

clean_stream:
  free(stream);
clean_file:
  free(file);
clean_up:
  return NULL;
}

/**
 * file_read reads the requested ammount of chars from buffer and returns,
 * unless there's less characters in stream.
 *
 * It returns the ammount of characters read when successful.
 * It returns -E_FILE_NOFILE when the file pointer isn't correct
 * It returns -E_FILE_NOBUFFER when no buffer found.
 */
size_t
file_read(file_t *file, size_t buffer_size, void *b)
{
  if (file == NULL)
    return -E_FILE_NOFILE;
  if (b == NULL)
    return -E_FILE_NOBUFFER;
  if (file->data == NULL)
    return -E_FILE_NOSTREAM;
  if (file->stream_cursor > file->file_size)
    return -E_FILE_COB;

  int chars_read = stream_read(file->data, file->stream_cursor, buffer_size, b);
  file -> stream_cursor += chars_read;
  return chars_read;
}