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
#include <fs/vfs.h>
#include <error/error.h>

directory *root = null;

int init_vfs(file *device, uint32_t inode)
{
  if (root != NULL)
    return -E_INIT;
  directory vroot = kalloc(sizeof(directory));
  if (vroot == NULL)
    return -E_INIT;
  memset(vroot, 0, sizeof(directory));
  vroot->no_entries = 0;
  vroot->entries = kalloc(sizeof(struct __ENTRIES));
  if (vroot == NULL)
  {
    free (vroot);
    return -E_INIT;
  }
  vroot->device = null;
  vroot->inode = 0;
  memset(vroot->entries, 0, sizeof(struct __ENTRIES));

  root = vroot;
  return -E_SUCCESS;
}

char** parse_path(char* path, char** buffer, uint32_t buffer_size)
{
  int i = 0;
  int j = 0;
  int k = 0;
  int escaped = 0;
  char* element = kalloc(MAX_NAME_LENGTH);
  memset(element, 0, MAX_NAME_LENGTH);
  for (; i < buffer_size; i++)
  {
    for (j = 0; j < 0xff; j++, k++)
    {
      switch(path[k])
      {
      case '\\':
        if (escaped == 1)
          element[j] = '\\';
        else
          escaped = 1;
        break;
      case '/':
        if (escaped == 1)
        {
          escaped = 0;
          element[j] = '/';
        }
        else
          goto next_element;
        break;
      case '\0':
        goto last_element;
      default:
        element[j] = path[k];
        break;
      }
    }
  next_element:
    buffer[i] = element;
    element = kalloc(MAX_NAME_LENGTH);
    memset(element, 0, MAX_NAME_LENGTH);
  }
last_element:
  buffer[i] = element;
  return buffer;
}

int make_dir(char* path, char* name)
{
  
}