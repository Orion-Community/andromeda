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

inode_t *_fs_root = NULL;

#define _FS_BMP_BITS (sizeof(int)*8)
#define _FS_BMP_IDX(a) (a/_FS_BMP_BITS)
#define _FS_BMP_OFF(a) (a%_FS_BMP_BITS)
#define _FS_BMP_SZ(a)  ((a%_FS_BMP_BITS == 0) ? a/_FS_BMP_BITS : a/_FS_BMP_BITS+1)

char* procPath = "proc";
char* devPath = "dev";

void fsInit(inode_t* root)
{
  if (root == NULL)
  {
//     panic("No root file system supplied!");
    printf("WARNING: File systems not complete!\n");
    _fs_root = kalloc(sizeof(inode_t));
    if (_fs_root == NULL) goto nomem;
    _fs_root -> inode = _FS_ROOT_INODE;
    _fs_root -> size  = 2*sizeof(struct _FS_DIR_ENTRY);
    _fs_root -> prot  = _FS_PROT_DIR | _FS_ROOT_RIGHTS;
    _fs_root -> usrid = 0;
    _fs_root -> grpid = 0;
    _fs_root -> data  = kalloc(sizeof(struct _FS_FILE));
    if (_fs_root -> data == NULL) goto nomem;
    _fs_root -> data -> start = kalloc (_fs_root -> size);
    if (_fs_root -> data -> start == NULL) goto nomem;
    _fs_root -> data -> end = (char*)((unsigned long) _fs_root -> data -> start + _fs_root -> size);
    _fs_root -> data -> read = _fs_root -> data -> start;
    _fs_root -> data -> write = _fs_root -> data -> start;
    
    struct _FS_DIR_ENTRY *root = (struct _FS_DIR_ENTRY*) _fs_root -> data -> start;
    root[0].drv = 0;
    root[0].inode = 0;
    root[0].virtInode = kalloc(sizeof(inode_t));
    root[0].nameSize = strlen(procPath);
    root[0].name = procPath;
    root[1].drv = 0;
    root[1].inode = 0;
    root[1].virtInode = kalloc(sizeof(inode_t));
    root[1].nameSize = strlen(devPath);
    root[1].name = devPath;
  }
  else
  {
    panic("File systems not yet supported!");
  }
  return;
nomem:
  panic("Not enough memory in fsInit!");
}

int mkdir (char* name, inode_t* parent, unsigned int prot, int usrid, int grpid)
{
  inode_t *tmp = kalloc(sizeof(inode_t));
  if (tmp == NULL) goto nomem;
  tmp -> prot  = _FS_PROT_DIR | prot;
  tmp -> drv   = parent -> drv;
  tmp -> inode = 0;
  tmp -> usrid = usrid;
  tmp -> grpid = grpid;
  tmp -> data  = kalloc(sizeof(FILE));
  if (tmp -> data == NULL) goto nomem;
  
  return 0;
  
nomem:
  panic("Not enough memory in mkdir");
  return -1;
}

void list(inode_t *dir)
{
  int size = dir -> size / sizeof (struct _FS_DIR_ENTRY);
  int i = 0;
  struct _FS_DIR_ENTRY *entries = (struct _FS_DIR_ENTRY *)dir -> data -> start;
  printf("%i Directory entries\n", size);
  for (; i < size; i++)
  {
    printf("%s\n",  entries[i].name);
  }
}

FILE* fopen()
{
  panic("fopen Requires a file system!");
}
