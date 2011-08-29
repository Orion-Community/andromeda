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
    printf("WARNING: File systems not complete!\n");
    /*
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

    */
    
    _fs_root = kalloc(sizeof(inode_t));
    if (_fs_root == NULL) goto nomem;
    _fs_root -> inode = _FS_ROOT_INODE;
    _fs_root -> size  = 0;
    _fs_root -> prot  = _FS_PROT_DIR | _FS_ROOT_RIGHTS;
    _fs_root -> usrid = 0;
    _fs_root -> grpid = 0;
    _fs_root -> data  = kalloc(sizeof(FILE));
    if (_fs_root -> data == NULL) goto nomem;
    _fs_root -> data -> start = NULL;
    _fs_root -> data -> end   = NULL;
    _fs_root -> data -> read  = NULL;
    _fs_root -> data -> write = NULL;
    _fs_root -> data -> buffered = TRUE;
    _fs_root -> data -> dirty = FALSE;
    _fs_root -> data -> size = &(_fs_root -> size);
    
    printf("Root size: %i\n", _fs_root -> size);
    
    mkdir ("proc", _fs_root, _FS_ROOT_RIGHTS, 0, 0);
    printf("Root size: %i\n", _fs_root -> size);
    mkdir ("dev",  _fs_root, _FS_ROOT_RIGHTS, 0, 0);
    printf("Root size: %i\n", _fs_root -> size);
    
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
  char* buf = kalloc(sizeof(struct _FS_DIR_ENTRY));
  
  inode_t *virtInode = kalloc(sizeof(inode_t));
  char* tmpName = kalloc(strlen(name)+1);
  memcpy(tmpName, name, strlen(name)+1);
  if (virtInode == NULL || tmpName == NULL) goto nomem;
  
  struct _FS_DIR_ENTRY dir = {parent -> drv, 0, virtInode, strlen(name), tmpName};
  
  write (parent -> data, &dir, sizeof(struct _FS_DIR_ENTRY));
  
  return 0;
  
nomem:
  panic("Not enough memory in mkdir");
  return -1;
}

int write (FILE* fp, char* buf, size_t num)
{
  if (fp -> buffered == FALSE) panic("Unbuffered files not supported yet!");
  if (fp -> start == NULL)
  {
    if (fp -> dirty)
      panic("Uninitialised dirty file!!!");
    fp -> start = kalloc(num);
    if (fp -> start == NULL) goto nomem;
    fp -> end     = (char*)((unsigned long)fp -> start + num);
    fp -> read    = fp -> start;
    fp -> write   = fp -> start;
    *(fp -> size) = num;
  }
  
  size_t readOffset  = (long)fp -> read -  (long)fp -> start;
  size_t writeOffset = (long)fp -> write - (long)fp -> start;
  
  printf("fp size %i\n", *fp->size);
  
  long toAdd = (long) fp -> write - (long) fp -> end + num;
  printf("To add: %i\n", toAdd);
  toAdd = (toAdd <= 0) ? 0 : toAdd;
  *(fp -> size) += toAdd;
  
  printf("fp size %i\n", *fp->size);
  
  char* tmp=realloc(fp->start, *fp->size);
  if (tmp != 0)
  {
    free(fp->start);
    fp -> start = tmp;
  }
  if (fp -> start == NULL) goto nomem;
  fp -> end   = (char*)((long)fp -> start + *fp -> size);
  fp -> read  = (char*)((long)fp -> start + readOffset);
  fp -> write = (char*)((long)fp -> start + writeOffset);
  
  int idx = 0;
  
  for (; idx < num; idx++)
  {
    *(char*)((long)fp -> write) = *(char*)((long)buf + idx);
    (long)fp -> write ++;
  }
  
  return 0;
  
  nomem:
  panic("Not enough memory in mkdir");
  return -1;
}

int read (FILE* fp, char* buf, size_t num)
{
  if (fp -> read > fp -> end || fp -> read > fp -> write)
    return -1;
  // We don't do anything yet!
  return -1;
}

int seek (FILE* fp, long offset, int from)
{
  unsigned long position = 0;
  switch (from)
  {
    case SEEK_SET:
      position = (unsigned long)fp -> start;
      break;
    case SEEK_RD:
      position = (unsigned long)fp -> read;
      break;
    case SEEK_WRT:
      position = (unsigned long)fp -> write;
      break;
    case SEEK_END:
      position = (unsigned long)fp -> end;
      break;
  }
  position += offset;
  if (position > (unsigned long)fp -> end)
    return -1;
  fp -> write = (char*)(position);
  fp -> read  = (char*)(position);
  return 0;
}

void list(inode_t *dir)
{
  int size = dir -> size / sizeof (struct _FS_DIR_ENTRY);
  int i = 0;
  struct _FS_DIR_ENTRY *entries = (struct _FS_DIR_ENTRY *)dir -> data -> start;
  printf("Entries: %X\n", entries);
  printf("Entries.name: %X\n", entries -> name);
  printf("%i Directory entries\n", size);
  for (; i < size; i++)
  {
    printf("%s\n",  entries[i].name);
  }
}

FILE* open()
{
  printf("Warning! Files can't be opened yet!\n");
  return NULL;
}

int close(FILE* fp)
{
  printf("Warining! File buffers can't be synchronised yet!\n");
  return -1;
}