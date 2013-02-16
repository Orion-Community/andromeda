/*
 *  Andromeda
 *  Copyright (C) 2013  Bart Kuivenhoven
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

#include <lib/tree.h>
#include <thread.h>

/**
 * \defgroup Stream
 * @{
 */

#ifndef __FS_STREAM_H
#define __FS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#define BLOCK_SIZE 0x1000

/**
 * \struct pipe_data_block
 * \var offset
 * \var next
 * \var prev
 * \var data
 */

struct pipe_data_block {
        unsigned int offset;

        struct pipe_data_block* next;
        struct pipe_data_block* prev;
        char data[1000];
};

/**
 * \struct pipe
 * \brief The pipe descriptor
 */
struct pipe {
        int reading_idx;
        int writing_idx;
        size_t block_size;

        atomic_t ref_cnt;
        mutex_t lock;

        struct tree_root* data;

        int (*close)(struct pipe*);
        int (*open)(struct pipe*);

        int (*write)(struct pipe*, char*);
        int (*read)(struct pipe*, char*, int);
        int (*flush)(struct pipe*);
};

#ifdef __cplusplus
}
#endif

#endif

/**
 * @} \file
 */
