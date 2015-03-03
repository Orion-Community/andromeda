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

#include <fs/vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

struct vfile;

typedef size_t (*fs_read_hook_t)(struct vfile* file, char* buf, size_t start, size_t len);
typedef size_t (*fs_write_hook_t)(struct vfile* file, char* buf, size_t start, size_t len);

#define MAX_PIPE_LEN 0x400000 /* Allow a maximum of 4MB */

#define BLOCK_SIZE 0x1000 /* Blocks are 4 KB in size */

/**
 * \struct pipe_data_block
 * \var offset
 * \var next
 * \var prev
 * \var data
 */

struct pipe_data_block {
        size_t offset;
        int dirty;

        char data[BLOCK_SIZE];
};

/**
 * \struct pipe
 * \brief The pipe descriptor
 */
struct pipe {
        uint32_t reading_idx;
        uint32_t writing_idx;
        size_t block_size;

        atomic_t ref_cnt;
        mutex_t lock;

        struct tree_root* data;

        int (*close)(struct pipe*);
        int (*open)(struct pipe*);

        int (*sync_write)(struct pipe*);
        int (*sync_read)(struct pipe*);
        struct vfile* output_file;
        struct vfile* input_file;

        int (*write)(struct pipe*, char*, size_t);
        int (*read)(struct pipe*, char*, size_t);
        int (*seek_write)(struct pipe*, int);
        int (*seek_read)(struct pipe*, int);
        int (*purge)(struct pipe*);
};

struct pipe* pipe_new(struct vfile* out_file, struct vfile* in_file);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @} \file
 */
