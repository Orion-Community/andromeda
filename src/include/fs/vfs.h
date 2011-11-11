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

#define MAX_NAME_LENGTH         0xff
#define VFS_DIR_ENTRIES         0x20
#define VFS_FILE_ENTRIES        VFS_DIR_ENTRIES

struct __DIR_ENTRY
{
  char name[MAX_NAME_LENGTH];
  uint32_t entry_type;
  union {
    struct __DIR *d;
    file *f;
  };
};

struct __DIR_ENTRIES
{
  struct __DIR_ENTRY entries[VFS_DIR_ENTRIES];

  struct __DIR_ENTRIES *next;
  struct __DIR_ENTRIES *prev;
};

struct __DIR
{
  file *device;
  uint32_t inode;

  struct __DIR_ENTRIES *entries;
};

typedef struct __DIR directory;

extern directory *root;

#endif