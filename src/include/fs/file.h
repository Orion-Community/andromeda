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
#include <fs/stream.h>

#ifndef __FS_FILE_H
#define __FS_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#define OWNER_READ      0x001
#define OWNER_WRITE     0x002
#define OWNER_EXECUTE   0x004

#define GROUP_READ      0x008
#define GROUP_WRITE     0x010
#define GROUP_EXECUTE   0x020

#define ALL_READ        0x040
#define ALL_WRITE       0X080
#define ALL_EXECUTE     0x100

#define VFILE_LIST_SIZE  0x400
#define VFILE_BLOCK_SIZE 0x1000

enum seektype {SEEK_SET, SEEK_CUR, SEEK_END};
enum list_type {blocks, lists};

struct __BLOCK_S;
struct __BLOCK_LIST_S;

struct __FILE_S
{
        char* path; /** Where is this file (if relevant) */
        uint64_t cursor_lo; /** 128-bits cursor in file */
        uint64_t cursor_hi;
        uint64_t file_size_lo; /** 128-bits file size */
        uint16_t file_size_hi;
        mutex_t file_lock; /** Is this file currently in use? */
        uint16_t rights; /** What are the rights */

        struct __BLOCK_S* blocks[0xC];

        /**
         * Did anyone ask for listception
         */

        struct __BLOCK_LIST_S *level0; /* Pointer to list of block pointers */
        struct __BLOCK_LIST_S *level1; /* Pointer to list of lists ... */
        struct __BLOCK_LIST_S *level2; /* Pointer to list of lists of lists ..*/

        struct __FILE_S *next;
};

struct __BLOCK_S
{
        char data[VFILE_BLOCK_SIZE];
};

struct __BLOCK_LIST_S
{
        enum list_type type;
        union
        {
                struct __BLOCK_S*       blocks [VFILE_LIST_SIZE];
                struct __BLOCK_LIST_S*  lists  [VFILE_LIST_SIZE];
        };
};

struct __FILE
{
  char *path;
  stream_t *data;
  uint16_t file_rights;
  uint64_t stream_cursor;
  uint64_t file_size;
};

typedef struct __FILE file_t;

file_t* file_open  (char *path);
int     file_close (file_t *file);
int     file_seek  (file_t *file, long long offset, enum seektype from);
size_t  file_read  (file_t *file, size_t buffer_size, void *b);
size_t  file_write (file_t *file, size_t buffer_size, void *b);
int     file_sync  (file_t *file);

#ifdef __cplusplus
}
#endif

#endif