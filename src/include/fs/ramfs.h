/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FS_RAMFS_H
#define __FS_RAMFS_H

#ifdef __cplusplus
extern "C" {
#endif

#define RAMFS_LIST_SIZE 0xFFFF

typedef enum {list, block_list} ramfs_list_type;

struct ramfs_blocks
{
        ramfs_list_type type;
        union {
                struct ramfs_block_list* lists[RAMFS_LIST_SIZE];
                struct ramfs_block* blocks[RAMFS_LIST_SIZE];
        };
};

struct ramfs_inode
{
        size_t file_size;
        /** List of blocks ... */
};

struct ramfs_block
{
        /** Actual block of data */
};

#ifdef __cplusplus
}
#endif

#endif