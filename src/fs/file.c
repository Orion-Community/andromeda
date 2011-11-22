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

  file->data = stream;
  return file;

clean_stream:
  free(stream);
clean_file:
  free(file);
clean_up:
  return NULL;
}

int
file_close(file_t *file)
{
  if (file == NULL)
    return -E_FILE_NOFILE;
  if (file->data == NULL)
    return -E_FILE_NOSTREAM;

  stream_close(file->data);
  free(file);
  return 0;
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

  int chars_read = stream_read(file->data, file->stream_cursor, buffer_size, b);
  file -> stream_cursor += (chars_read >= 0) ? chars_read : 0;
#ifdef STREAM_DBG
  printf("File_read: %s\n", b);
#endif
  return chars_read;
}

int file_seek(file_t *file, long long offset, enum seektype from)
{
  if (file == NULL)
    return -E_FILE_NOFILE;
  switch (from)
  {
    case SEEK_SET:
      if (offset < 0)
      {
        file->stream_cursor = 0;
        break;
      }
      file->stream_cursor = (offset <= file->file_size)
                                                     ? offset : file->file_size;
      break;
    case SEEK_CUR:
      if (offset < 0)
      {
        file->stream_cursor += (offset - file->stream_cursor >= 0)
                                                 ? offset : file->stream_cursor;
        break;
      }

      file->stream_cursor += (offset + file->stream_cursor <= file->file_size)
                                                     ? offset : file->file_size;
      break;
    case SEEK_END:
      if (offset > 0)
      {
        file->stream_cursor = file->file_size;
        break;
      }
      file->stream_cursor = (offset >= file->file_size) ? 0 : offset;
      break;
    default:
      return -1;
      break;
  }
  return 0;
}

/**
 * file_write writes the requested ammount of chars in buffer and from buffer
 * and returns, unless there's been an allocation error.
 *
 * It returns the ammount of characters read when successful.
 * It returns -E_FILE_NOFILE when the file pointer isn't correct
 * It returns -E_FILE_NOBUFFER when no buffer found.
 */

size_t
file_write(file_t *file, size_t buffer_size, void *b)
{
   if (file == NULL)
    return -E_FILE_NOFILE;
  if (b == NULL)
    return -E_FILE_NOBUFFER;
  if (file->data == NULL)
    return -E_FILE_NOSTREAM; 
#ifdef STREAM_DBG
  printf("File_write: %s\n", b);
#endif
  uint32_t growth;
  int chars_written = stream_write(file->data,
                                   file->stream_cursor, buffer_size, b, &growth);
  file -> stream_cursor += (chars_written >= 0) ? chars_written : 0;
  file -> file_size += growth;
  return chars_written;
}