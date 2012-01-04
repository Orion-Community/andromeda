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

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_LIST_SIZE 0xFF
#define BUFFER_BLOCK_SIZE 0x1000

typedef enum {lists, blocks} buffer_list_t;

struct buffer_block
{
        char data[BUFFER_BLOCK_SIZE];
};

struct buffer_list
{
        buffer_list_t type;
        union
        {
                struct buffer_block* blocks[BUFFER_LIST_SIZE];
                struct buffer_list*  lists[BUFFER_LIST_SIZE];
        };
};

struct buffer
{
        idx_t index;
        size_t buffer_size;

        struct buffer_block* direct[BUFFER_LIST_SIZE];
        struct buffer_list* single_indirect;
        struct buffer_list* double_indirect;
        struct buffer_list* triple_indirect;

        int (*read)(struct buffer* this, char* buf, size_t num);
        int (*write)(struct buffer* this, char* buf, size_t num);
        int (*close)(struct buffer* this);
};

#ifdef __cplusplus
}
#endif

#endif
