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

#include <fs/fs.h>
#include <fs/file.h>
#include <stdlib.h>

struct _FS_INODE* _fs_root = NULL;

#define _FS_BMP_BITS (sizeof(int)*8)
#define _FS_BMP_IDX(a) (a/_FS_BMP_BITS)
#define _FS_BMP_OFF(a) (a%_FS_BMP_BITS)
#define _FS_BMP_SZ(a)  ((a%_FS_BMP_BITS == 0) ? a/_FS_BMP_BITS : a/_FS_BMP_BITS+1)

/*
 * /proc
 * /dev
 */

void fsInit(inode_t* root)
{
  if (root == NULL)
  {
    panic("No root file system supplied!");
    _fs_root = kalloc(sizeof(inode_t));
    _fs_root -> inode = _FS_ROOT_INODE;
    _fs_root -> size  = 2*sizeof(struct _FS_DIR_ENTRY);
    _fs_root -> data  = kalloc(sizeof(struct _FS_FILE));
    _fs_root -> data -> start = kalloc (_fs_root -> size);
    _fs_root -> data -> end = (char*)((unsigned long) _fs_root -> data -> start + _fs_root -> size);
    _fs_root -> data -> read = _fs_root -> data -> start;
    _fs_root -> data -> write = _fs_root -> data -> start;
  }
  else
  {
    panic("File systems not yet supported!");
  }
}

FILE* fopen()
{
  panic("fopen Requires a file system!");
}
