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

#ifndef __FS_FS_H
#define __FS_FS_H

#include<fs/file.h>

#define _FS_PROT_DIR   0x200
#define _FS_PROT_OWN_R 0x100
#define _FS_PROT_OWN_W 0x080
#define _FS_PROT_OWN_X 0x040
#define _FS_PROT_GRP_R 0x020
#define _FS_PROT_GRP_W 0x010
#define _FS_PROT_GRP_X 0x008
#define _FS_PROT_ALL_R 0x004
#define _FS_PROT_ALL_W 0x002
#define _FS_PROT_ALL_X 0x001

struct _FS_INODE
{
  char* name;                // File name
  unsigned short protection; // protection bits d-rwx-rwx-rwx
  unsigned int userid;       // Owner data
  unsigned int groupid;      // Owner data
  unsigned int meta;         // Any flags required for administration
  unsigned int device;       // Device ID
  unsigned int inode;        // Address of inode on disk
  unsigned int length;       // Length of the file in units of 512 bytes
  struct _FS_INODE* poiter;  // For symlinks
};

extern struct _FS_INODE* _fs_root;

#endif