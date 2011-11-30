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
#include <fs/stream.h>
#include <fs/path.h>

void file_test(char* data)
{
  printf("%s\n", data);
  file_t *file = file_open(NULL);
  file_write(file, strlen(data), data);
  char *s = kalloc(strlen(data) + 1);
  memset(s, 0, strlen(data) + 1);
  file_seek(file, 0, SEEK_SET);
  file_read(file, strlen(data), s);
  printf("%s\n", s);
  free(s);
  file_close(file);
}

void large_file_test()
{
  file_t *file = file_open(NULL);
  char *c =  "ABCD";
  int idx = 0;
  for (; idx < 0x1000; idx++)
  {
    printf("%x\n", idx);
    file_write(file, strlen(c), c);
  }
  file_seek(file, 0, SEEK_SET);
  demand_key();
  int r = 8;
  char *d = kalloc(r);
  memset(d, 0, r);
  for (idx = 0; idx < 0x400; idx++)
  {
    file_read(file, r-1, d);
    printf("%x\t%s\n", idx, d);
  }
  free (d);
  file_close(file);
}

void path_test(char *path)
{
  printf("Path: %s\n", path);
  struct __PATH_ELEMENT *path_list = parse_path(path);
  struct __PATH_ELEMENT *carriage = path_list;
  for (; carriage != NULL; carriage = carriage->next)
  {
    printf("Element text: %s\n", carriage->name);
  }
  clean_path(path_list);
  printf("\n");
}