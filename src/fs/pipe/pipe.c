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
                BLOCK_SIZE, NULL, NULL);
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
                return kmalloc(pipe->block_size);
        }
#else
        return kmalloc(pipe->block_size);
#endif
}

static int pipe_cleanup_block(void *data, void* block)
{
        struct pipe *pipe = (struct pipe*) data;
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

/**
 * \fn pipe_flush
 * \brief Clean the pipe buffer
 */
static int pipe_flush(struct pipe* pipe)
{
        if (pipe == NULL)
                return -E_NULL_PTR;

        if (pipe->data == NULL)
                return -E_SUCCESS;

        int tmp = pipe->data->flush(pipe->data, pipe_cleanup_block, pipe);

        if (tmp == -E_SUCCESS)
                pipe->data = NULL;
        return tmp;
}

static void* pipe_get_block(struct pipe* pipe, int key)
{
        if (pipe == NULL || pipe->data == NULL || pipe->data->find == NULL)
                return NULL ;

        void* block = pipe->data->find(key, pipe->data);
        if (block == NULL) {
                block = pipe_get_new_block(pipe);
                if (block == NULL)
                        return NULL ;
                pipe->data->add(key, block, pipe->data);
        }
        return block;
}

/**
 * \fn pipe_read
 * \brief Read from pipe
 */
static int pipe_read(struct pipe* pipe, char* data, size_t len)
{
        if (pipe == NULL || data == NULL || len == 0) {
                return -E_INVALID_ARG;
        }

        /* Prevent race conflicts */
        mutex_lock(&pipe->lock);

        /* Set up the counters */
        int key = pipe->reading_idx / pipe->block_size;
        void* block = pipe_get_block(pipe, key);

        /* Make sure we're not working with garbage */
        if (block == NULL) {
                goto err;
        }

        /* Get the right offset */
        size_t offset = pipe->reading_idx % pipe->block_size;
        size_t i = 0;

        /* Do the copying bit */
        /** \todo Implement using memcpy, can be much simpler and faster */
        for (; i < len; i++, pipe->reading_idx++) {
                data[i] = ((char*) block)[offset];

                if (++offset >= pipe->block_size) {
                        block = pipe_get_block(pipe, ++key);
                        if (block == NULL)
                                goto err_read;
                        offset = 0;
                }
        }

        err_read:
        mutex_unlock(&pipe->lock);
        return i;

        err: mutex_unlock(&pipe->lock);
        return -E_NULL_PTR;
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
        void* block = pipe_get_block(pipe, key);
        if (block == NULL) {
                goto err;
        }

        size_t i = 0;
        for (; i <= len; i++, pipe->writing_idx++) {
                /* If we violated the block size */
                if (offset + i >= pipe->block_size) {
                        /* get the next block */
                        key++;
                        offset = 0;
                        block = pipe_get_block(pipe, key);
                        if (block == NULL)
                                goto err_written;
                }
                /* Write the next character */
                ((char*) block)[offset + i] = ((char*) data)[i];
        }

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
                pipe_flush(pipe);
                kfree(pipe);
        }

        return -E_SUCCESS;
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

/**
 * \fn pipe_new
 * \brief Construct a new pipe
 */
struct pipe* pipe_new()
{
        struct pipe* p = kmalloc(sizeof(*p));
        if (p == NULL)
                return NULL ;

        memset(p, 0, sizeof(*p));

        p->read = pipe_read;
        p->write = pipe_write;
        p->close = pipe_close;
        p->open = pipe_open;
        p->flush = pipe_flush;

        p->data = tree_new_avl();

        return NULL ;
}

/**
 * @}
 * \file
 */
