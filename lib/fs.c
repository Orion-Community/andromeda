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
struct _FS_FILE* _fs_drives[_FS_MAX_DRIVES];

void fsInitDrives()
{   
  int i = 0;
  for (; i < _FS_MAX_DRIVES; i++)
  {
    _fs_drives[i] = NULL;
  }
}

#define _FS_BMP_BITS (sizeof(int)*8)
#define _FS_BMP_IDX(a) (a/_FS_BMP_BITS)
#define _FS_BMP_OFF(a) (a%_FS_BMP_BITS)
#define _FS_BMP_SZ(a)  ((a%_FS_BMP_BITS == 0) ? a/_FS_BMP_BITS : a/_FS_BMP_BITS+1)

void fsInit(inode_t* root)
{
  if (root == NULL)
  {
    _fs_root = memFsInit(_VFS_STD_SIZE, _FS_ROOT_RIGHTS);
    _fs_root->root = (void*)((unsigned long)_fs_root + sizeof(struct _FS_INODE));
    _fs_root->root->bmp = (void*)((unsigned long)_fs_root->root + sizeof(struct _FS_ROOTNODE));
    _fs_root->root->root = _fs_root;
    _fs_root->root->size = _VFS_STD_SIZE;
    _fs_root->root->free = _fs_root->root->size - (sizeof(struct _FS_INODE) + sizeof(struct _FS_ROOTNODE) + _FS_BMP_SZ(_VFS_STD_SIZE));
    _fs_root->root->mounts = 0;
    int i = 0;
    for (; i < _VFS_STD_SIZE; i++)
    {
      _fs_root->root->bmp[_FS_BMP_IDX(i)] |= (i > _VFS_STD_SIZE-_fs_root->root->free) ? 1 << _FS_BMP_OFF(i) : 0 << _FS_BMP_OFF(i);
    }
    _fs_root->meta = _FS_META_ROOT;
    _fs_root->name = "/";
    _fs_root->offset = (unsigned long)_fs_root->root->bmp + _FS_BMP_SZ(_VFS_STD_SIZE);
  }
}

struct _FS_INODE* memFsInit(size_t size, unsigned int protection)
{
  int device = 0;
  for (; device <= _FS_MAX_DRIVES && _fs_drives[device] != NULL; device++);
  if (device == _FS_MAX_DRIVES)
    panic("No more free drives to use in vfsInit");
  _fs_drives[device] = nalloc(sizeof(struct _FS_FILE));
  if (_fs_drives == NULL )
  {
    panic("Out of memory in vfsInit!");
  }
  _fs_drives[device]->start   = kalloc(size*_VFS_STD_BLCK);
  _fs_drives[device]->end     = (char*)((size_t)_fs_drives[device]->start + size);
  _fs_drives[device]->read    = _fs_drives[device]->start;
  _fs_drives[device]->write   = _fs_drives[device]->start;
  _fs_drives[device]->size    = size;
  
  struct _FS_INODE* vfs  = (struct _FS_INODE*)_fs_drives[device]->start;
  vfs -> name            = NULL;
  vfs -> protection      = protection;
  vfs -> userid          = _FS_USER_ROOT;
  vfs -> groupid         = _FS_GROUP_ROOT;
  vfs -> meta            = 0;
  vfs -> device          = device;
  vfs -> inode           = 0; // Standard file size = 0 bytes
  vfs -> length          = 0;
  vfs -> offset          = sizeof(struct _FS_INODE);
  return vfs;
}