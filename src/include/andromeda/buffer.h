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

#ifndef __ANDROMEDA_BUFFER_H
#define __ANDROMEDA_BUFFER_H

#include <fs/vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_LIST_SIZE 0xFF
#define BUFFER_BLOCK_SIZE 0x1000

#define BUFFER_ALLOW_DUPLICATE (1<<0)
#define BUFFER_ALLOW_GROWTH    (1<<1)

typedef enum {lists, blocks} buffer_list_t;

struct buffer_block
{
        mutex_t lock;
        char data[BUFFER_BLOCK_SIZE];
};

struct buffer_list
{
        buffer_list_t type; /** Are we using branches or are these leaves */
        atomic_t used; /** How many entries are currently in use */
        mutex_t lock; /** Used when adding or removing a leaf/branch */
        union
        {
                struct buffer_block* blocks[BUFFER_LIST_SIZE]; /** The leaves */
                struct buffer_list*  lists[BUFFER_LIST_SIZE]; /** The branches*/
        };
};

struct buffer
{
        mutex_t lock;
        size_t  size;
        size_t  base_idx;

        uint8_t rights;

        struct buffer_block* direct[BUFFER_LIST_SIZE];
        struct buffer_list* single_indirect;
        struct buffer_list* double_indirect;
        struct buffer_list* triple_indirect;

        atomic_t opened;
        idx_t cursor;

        struct buffer* (*duplicate)(struct buffer* this);
        int (*read)(struct buffer* this, char* buf, size_t num);
        int (*write)(struct buffer* this, char* buf, size_t num);
        int (*seek)(struct buffer* this, long offset, seek_t from);
        int (*close)(struct buffer* this);
};

struct buffer* buffer_init();

#ifdef __cplusplus
}
#endif

#endif
