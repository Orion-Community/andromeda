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

#ifndef __FS_FILE_H
#define __FS_FILE_H

#include <types.h>

struct _FS_FILE
{
  char* start; // Start of file (within this inode)
  char* end;   // End of file (within this inode)
  char* read;  // The read pointer (within this inode)
  char* write; // The write pointer (within this inode, should be >= read)
  size_t size; // Size of this inode
  struct _FS_FILE* chain; // Next inode, if this one isn't large enough
  char* path;  // Path to file
  boolean buffered; // TRUE for buffered
};

typedef struct _FS_FILE FILE;

struct _FS_DIR_ENTRY
{
  unsigned int drv;
  unsigned int inode;
  struct _FS_INODE* virtInode;
  size_t nameSize;
  char *name;
};

#endif
