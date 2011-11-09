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

#ifndef __FS_VFS_H
#define __FS_VFS_H

#include <fs/file.h>

#define MAX_NAME_LENGTH 0xff

struct __DIR_ENTRY
{
  char name[MAX_NAME_LENGTH];
  struct __DIR *ptr;
};

struct __ENTRIES
{
  struct __DIR_ENTRY entries[0x20];

  struct __ENTRIES *next;
  struct __ENTRIES *prev;
};

struct __DIR
{
  file *device;
  uint32_t inode;

  struct __ENTRIES *entries;
  uint32_t no_entries;
};

typedef struct __DIR directory;

extern directory *root;

#endif