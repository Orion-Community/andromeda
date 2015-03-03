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

#include <stdio.h>
#include <stdlib.h>
#include <fs/pipe.h>
#include <mm/cache.h>
#include <andromeda/system.h>

/**
 * \addtogroup Stream
 * @{
 */

#ifdef SLAB
static struct mm_cache* pipe_cache = NULL;
#endif

static void* pipe_get_new_block(struct pipe* pipe)
{
#ifdef SLAB
        if (pipe_cache == NULL) {
                pipe_cache = mm_cache_init("pipe blocks", BLOCK_SIZE,
                                sizeof(struct pipe_data_block), NULL, NULL);
        }
        if (pipe_cache == NULL) {
                return NULL ;
        }
#endif

        if (pipe == NULL) {
                return NULL ;
        }

#ifdef SLAB
        if (pipe->block_size == BLOCK_SIZE) {
                return mm_cache_alloc(pipe_cache, 0);
        } else {
                return kmalloc(sizeof(struct pipe_data_block));
        }
#else
        return kmalloc(sizeof(struct pipe_data_block));
#endif
}

static int pipe_cleanup_block(struct pipe* pipe, void* block)
{
        if (pipe == NULL || block == NULL)
                return -E_NULL_PTR;

        int ret = -E_SUCCESS;
#ifdef SLAB
        if (pipe->block_size != BLOCK_SIZE)
                kfree(block);
        else
                ret = mm_cache_free(pipe_cache, block);
#else
        kfree(block);
#endif

        return ret;
}

static int pipe_cleanup_block_wrapper(void* pipe, void* block)
{
        return pipe_cleanup_block(pipe, block);
}

/**
 * \fn pipe_flush
 * \brief Clean the pipe buffer
 */
static int pipe_purge(struct pipe* pipe)
{
        if (pipe == NULL)
                return -E_NULL_PTR;

        if (pipe->data == NULL)
                return -E_SUCCESS;

        int tmp = pipe->data->purge(pipe->data, pipe_cleanup_block_wrapper,
                        pipe);

        if (tmp == -E_SUCCESS)
                pipe->data = NULL;
        return tmp;
}

static int pipe_clean_blocks(struct pipe* pipe)
{
        if (pipe == NULL) {
                return -E_NULL_PTR;
        }

        int key = (pipe->writing_idx - MAX_PIPE_LEN) / pipe->block_size;
        struct pipe_data_block* block = pipe->data->find_smaller(key,
                        pipe->data);
        while (block != NULL ) {
                size_t idx = block->offset / pipe->block_size;

                pipe->data->delete(idx, pipe->data);
                pipe_cleanup_block(pipe, block);

                block = pipe->data->find_smaller(key, pipe->data);
        }

        /** \todo Do block relocation to make the indices smaller, if pipe is not connected to a file system */
        return -E_SUCCESS;
}

static struct pipe_data_block* pipe_get_block(struct pipe* pipe, int key,
                int dirty)
{
        if (pipe == NULL || pipe->data == NULL || pipe->data->find == NULL)
                return NULL ;

        struct pipe_data_block* block = pipe->data->find(key, pipe->data);
        if (block == NULL) {
                block = pipe_get_new_block(pipe);
                if (block == NULL)
                        return NULL ;
                pipe->data->add(key, block, pipe->data);
                block->offset = key * pipe->block_size;
                block->dirty = dirty;
        }
        return block;
}

/**
 * \fn pipe_read
 * \brief Read from pipe
 */
static inline int pipe_read_locked(struct pipe* pipe, char* data, size_t len)
{
        if (pipe == NULL || data == NULL || len == 0) {
                return -E_INVALID_ARG;
        }

        /* Set up the counters */
        int key = pipe->reading_idx / pipe->block_size;
        struct pipe_data_block* block_hdr = pipe_get_block(pipe, key, 0);
        void* block = &block_hdr->data;

        /* Make sure we're not working with garbage */
        if (block_hdr == NULL) {
                return -E_NULL_PTR;
        }

        /* Get the right offset */
        size_t offset = pipe->reading_idx % pipe->block_size;
        size_t i = 0;

        /* Do the copying bit */
        /** \todo Implement using memcpy, can be much simpler and faster */
        for (; i < len; i++, pipe->reading_idx++, offset++) {
                /* Make sure we're not reading garbage */
                if (offset >= pipe->block_size) {
                        block_hdr = pipe_get_block(pipe, ++key, 0);
                        block = &block_hdr->data;
                        if (block_hdr == NULL) {
                                return i;
                        }
                        offset = 0;
                }
                /* Also, make sure we don't overtake the input */
                if (pipe->reading_idx + i >= pipe->writing_idx) {
                        break;
                }
                /* Break in case of integer overflow */
                if (pipe->reading_idx + i + 1 < pipe->reading_idx) {
                        break;
                }
                data[i] = ((char*) block)[offset];
        }

        return i;
}

static inline int pipe_read(struct pipe* pipe, char* data, size_t len)
{
        if (pipe == NULL || data == NULL || len == 0) {
                return -E_INVALID_ARG;
        }

        mutex_lock(&pipe->lock);

        /* Do the reading bit */
        int ret = pipe_read_locked(pipe, data, len);

        /* Advance the index */
        pipe->reading_idx += ret;

        /* Do some clean up */
        pipe_clean_blocks(pipe);

        mutex_unlock(&pipe->lock);

        return ret;
}

/**
 * \fn pipe_write
 * \brief Write to pipe
 */
static int pipe_write(struct pipe* pipe, char* data, size_t len)
{
        if (pipe == NULL || data == NULL)
                return -E_NULL_PTR;

        if (pipe->block_size == 0) {
                panic("Divide by zero");
        }

        /**
         * \todo Implement using memcpy, can be much faster and simpler
         */

        /* Prevent some race conflicts */
        mutex_lock(&pipe->lock);

        /* Prepare the counters */
        int key = pipe->writing_idx / pipe->block_size;
        size_t offset = pipe->writing_idx % pipe->block_size;

        /* Get the first block to write to */
        struct pipe_data_block* block_hdr = pipe_get_block(pipe, key, 1);
        void* block = &block_hdr->data;
        if (block_hdr == NULL) {
                goto err;
        }

        block_hdr->dirty = 1;
        size_t i = 0;
        for (; i <= len; i++, pipe->writing_idx++) {
                /* If we violated the block size */
                if (offset + i >= pipe->block_size) {
                        /* get the next block */
                        key++;
                        offset = 0;
                        block_hdr = pipe_get_block(pipe, key, 1);
                        block = &block_hdr->data;
                        if (block_hdr == NULL) {
                                goto err_written;
                        }
                        block_hdr->dirty = 1;
                }

                /* Check if we violated pipe length constraints */
                if (pipe->writing_idx + i - pipe->reading_idx >= MAX_PIPE_LEN) {
                        break;
                }
                /* Just stop when doing an integer overflow */
                /** \todo Figure out a way to prevent integer overflows */
                if (pipe->writing_idx + i + 1 < pipe->writing_idx) {
                        break;
                }
                /* Write the next character */
                ((char*) block)[offset + i] = ((char*) data)[i];
        }
        pipe->writing_idx += i;

        err_written: mutex_unlock(&pipe->lock);
        return i;

        err: mutex_unlock(&pipe->lock);

        return 0;

}

/**
 * \fn pipe_close
 * \brief Update reference count and if necessary clean up
 */
static int pipe_close(struct pipe* pipe)
{
        if (pipe == NULL)
                return -E_NULL_PTR;

        if (atomic_dec(&pipe->ref_cnt) == 0) {
                pipe_purge(pipe);
                kfree(pipe);
        }

        return -E_SUCCESS;
}

static int pipe_update_block(struct pipe* this, size_t idx)
{
        if (this == NULL || this->input_file == NULL
            || this->input_file->fs_data.read == NULL) {
                return -E_NULL_PTR;
        }

        int key = idx / this->block_size;
        struct pipe_data_block* block = pipe_get_block(this, key, 0);
        if (block == NULL) {
                return -E_NULL_PTR;
        }

        return this->input_file->fs_data.read(this->input_file,
                        ((char*) &(block->data)), block->offset,
                        this->block_size);
}

static int pipe_seek_read(struct pipe* this, int shift)
{
        if (this == NULL) {
                return 0;
        }
        if (shift == 0) {
                return 0;
        }

        mutex_lock(&this->lock);

        /* Do the shift */
        int64_t read_idx = this->reading_idx + shift;

        /* Make sure we're not doing a buffer underflow */
        if (read_idx < 0) {
                read_idx = 0;
        }
        /* Handle integer overflow */
        if (((int32_t) read_idx) < 0) {
                read_idx = 0x7FFFFFFF; /* Last positive value before overflow */
        }
        /* Make sure we don't exceed the pipe length */
        if ((this->writing_idx - read_idx) > MAX_PIPE_LEN) {
                int overflow = (this->writing_idx - read_idx) - MAX_PIPE_LEN;
                read_idx += overflow;
        }
        /* Make sure we don't overtake the writing cursor */
        if (read_idx > this->writing_idx) {
                read_idx = this->writing_idx;
        }

        int32_t shifted = read_idx - this->reading_idx;
        if (shifted < 0) {
                size_t idx = read_idx;
                for (; idx < this->reading_idx; idx += this->block_size) {
                        pipe_update_block(this, idx);
                }
        }
        this->reading_idx = read_idx;
        mutex_unlock(&this->lock);
        return shifted;
}

static int pipe_seek_write(struct pipe* this, int shift)
{
        if (this == NULL || shift == 0) {
                return 0;
        }

        mutex_lock(&this->lock);

        int64_t write_idx = this->writing_idx + shift;
        /* Don't go into actual garbage */
        if (write_idx < 0) {
                write_idx = 0;
        }
        /* Or integer overflow garbage */
        if (((int32_t) write_idx) < 0) {
                write_idx = 0x7FFFFFFF; /* Last positive value before overflow */
        }
        /* Make sure we don't end up before the reading index */
        if (write_idx < this->reading_idx) {
                write_idx = this->reading_idx;
        }

        /* Make sure we don't exceed pipe length */
        if ((write_idx - this->reading_idx) > MAX_PIPE_LEN) {
                size_t overflow = (write_idx - this->reading_idx) - MAX_PIPE_LEN;
                write_idx -= overflow;
        }

        int32_t shifted = write_idx - this->writing_idx;
        if (shifted > 0) {
                /* Make sure all the intermediate blocks are in the pipe */
                int idx = this->writing_idx;
                for (; idx <= write_idx; idx += this->block_size) {
                        pipe_get_block(this, idx, 0);
                }
        }
        this->writing_idx = write_idx;

        mutex_unlock(&this->lock);

        return shifted;
}

/**
 * \fn pipe_open
 * \brief Update reference count
 */
static int pipe_open(struct pipe* pipe)
{
        if (pipe == NULL)
                return -E_NULL_PTR;

        atomic_inc(&pipe->ref_cnt);
        return -E_SUCCESS;
}

static int pipe_sync_read(struct pipe* pipe)
{
        if (pipe == NULL || pipe->input_file == NULL
            || pipe->input_file->fs_data.read == NULL) {
                return -E_NULL_PTR;
        }

        mutex_lock(&pipe->input_file->file_lock);
        mutex_lock(&pipe->lock);

        uint32_t idx = pipe->reading_idx;
        for (; idx <= pipe->writing_idx; idx += pipe->block_size) {
                pipe_update_block(pipe, idx);

        }

        mutex_unlock(&pipe->input_file->file_lock);
        mutex_unlock(&pipe->lock);

        return -E_SUCCESS;
}

static int pipe_sync_write(struct pipe* pipe)
{
        if (pipe == NULL || pipe->output_file == NULL
            || pipe->output_file->fs_data.write == NULL) {
                return 0;
        }

        int written = 0;

        mutex_lock(&pipe->output_file->file_lock);
        mutex_lock(&pipe->lock);

        /* If the input and output are the same, we can do some skipping */
        if (pipe->input_file == pipe->output_file) {
                /* Get the buffer ready */
                char* buf = kmalloc(pipe->block_size);
                if (buf == NULL) {
                        written = -E_OUT_OF_RESOURCES;
                        goto err;
                }

                /* Make sure we're aligned with the block boundary */
                int mod = pipe->reading_idx & pipe->block_size;
                if (mod != 0) {
                        pipe->reading_idx -= mod;
                }
                /* Iterate through the blocks for testing and copying */
                while (pipe->reading_idx < pipe->writing_idx) {
                        /* get block data */
                        struct pipe_data_block* block = pipe_get_block(pipe,
                                        pipe->reading_idx / pipe->block_size,
                                        0);

                        /* If this can be skipped, do so */
                        size_t diff = pipe->writing_idx - pipe->reading_idx;
                        if (block->dirty == 0 && diff >= pipe->block_size) {
                                written += pipe->block_size;
                                pipe->reading_idx += pipe->block_size;
                                continue;
                        }
                        /* Read to the buffer */
                        pipe_read_locked(pipe, buf, pipe->block_size);

                        /* Now write to the callback */
                        written += pipe->output_file->fs_data.write(
                                        pipe->output_file, buf,
                                        pipe->reading_idx, pipe->block_size);

                        /* Update the index */
                        pipe->reading_idx += written;
                        kfree_s(buf, pipe->block_size);
                }
        } else {
                /* We can't skip so here goes everything! */
                size_t buf_size = pipe->writing_idx - pipe->reading_idx;
                char* buf = kmalloc(buf_size);
                if (buf == NULL) {
                        written = -E_OUT_OF_RESOURCES;
                        goto err;
                }

                size_t read = pipe_read_locked(pipe, buf, buf_size);
                written += pipe->output_file->fs_data.write(pipe->output_file,
                                buf, pipe->reading_idx, read);

                pipe->reading_idx += written;
                kfree_s(buf, buf_size);
        }

        err: mutex_lock(&pipe->output_file->file_lock);
        mutex_unlock(&pipe->lock);

        return written;
}

/**
 * \fn pipe_new
 * \brief Construct a new pipe
 */
struct pipe* pipe_new(struct vfile* in_file, struct vfile* out_file)
{
        struct pipe* p = kmalloc(sizeof(*p));
        if (p == NULL)
                return NULL ;

        memset(p, 0, sizeof(*p));

        p->read = pipe_read;
        p->write = pipe_write;
        p->seek_read = pipe_seek_read;
        p->seek_write = pipe_seek_write;
        p->close = pipe_close;
        p->open = pipe_open;
        p->purge = pipe_purge;
        p->sync_write = pipe_sync_write;
        p->sync_read = pipe_sync_read;
        p->output_file = out_file;
        p->input_file = in_file;

        p->data = tree_new_avl();

        return p;
}

/**
 * @}
 * \file
 */
