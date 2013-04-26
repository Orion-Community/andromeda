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
 * \AddToGroup Stream
 * @{
 */

static struct mm_cache* pipe_cache = NULL;

static void* pipe_get_new_block(struct pipe* pipe)
{
#ifdef SLAB
        if (pipe_cache == NULL)
                pipe_cache = mm_cache_init("pipe blocks", BLOCK_SIZE, BLOCK_SIZE,NULL, NULL);
        if (pipe_cache == NULL)
                return NULL;
#endif

        if (pipe == NULL)
                return NULL;

#ifdef SLAB
        if (pipe->block_size == BLOCK_SIZE)
                return mm_cache_alloc(pipe_cache, 0);
        else
                return kmalloc(pipe->block_size);
#else
        return kmalloc(pipe->block_size);
#endif
}

static int pipe_cleanup_block(void *data, void* block)
{
        struct pipe *pipe = (struct pipe*)data;
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
                return NULL;

        struct tree* t = pipe->data->find(key, pipe->data);
        if (t == NULL)
        {
                void* block = pipe_get_new_block(pipe);
                if (block == NULL)
                        return NULL;
                pipe->data->add(key, block, pipe->data);
                return block;
        }
        return t->data;
}

/**
 * \fn pipe_read
 * \brief Read from pipe
 */
static int pipe_read(struct pipe* pipe, char* data, int len)
{
        if (pipe == NULL || data == NULL || len == 0)
                return -E_INVALID_ARG;

        mutex_lock(&pipe->lock);
        int key = pipe->reading_idx/pipe->block_size;
        void* block = pipe_get_block(pipe, key);
        if (block == NULL)
                return -E_NULL_PTR;

        int offset = pipe->reading_idx % pipe->block_size;
        int i = 0;
        for (; i < len; i++, pipe->reading_idx++)
        {
                if (++offset >= pipe->block_size)
                {
                        block = pipe_get_block(pipe, ++key);
                        if (block == NULL)
                                goto err;
                        offset = 0;
                }
                data[i] = ((char*)block)[offset];
        }

        /* Maybe do some reading bits here? */

        mutex_unlock(&pipe->lock);
        return -E_NOFUNCTION;
err:
        mutex_unlock(&pipe->lock);
        return -E_NULL_PTR;
}

/**
 * \fn pipe_write
 * \brief Write to pipe
 */
static int pipe_write(struct pipe* pipe, char* data)
{
        if (pipe == NULL || data == NULL)
                return -E_NULL_PTR;

        mutex_lock(&pipe->lock);
        int key = pipe->writing_idx / pipe->block_size;
        int offset = pipe->writing_idx % pipe->block_size;

        void* block = pipe_get_block(pipe, key);

        int i = 0;
        for (;!(data[i] == '\0' && data[i+1] != '\0'); i++, pipe->writing_idx++)
        {
                if (offset+i >= pipe->block_size)
                {
                        key++;
                        offset = 0;
                        block = pipe_get_block(pipe, key);
                        if (block == NULL)
                                goto err;
                }
                ((char*)block)[offset+i] = ((char*)data)[i];
        }

        mutex_unlock(&pipe->lock);
        /* Now do some writing bits ... */

        return -E_SUCCESS;
err:
        mutex_unlock(&pipe->lock);
        return -E_NULL_PTR;
}

/**
 * \fn pipe_close
 * \brief Update reference count and if necessary clean up
 */
static int pipe_close(struct pipe* pipe)
{
        if (pipe == NULL)
                return -E_NULL_PTR;

        if (atomic_dec(&pipe->ref_cnt) == 0)
        {
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
                return NULL;

        memset(p, 0, sizeof(*p));

        p->read = pipe_read;
        p->write = pipe_write;
        p->close = pipe_close;
        p->open = pipe_open;
        p->flush = pipe_flush;

        p->data = tree_new_avl();

        return NULL;
}

/**
 * @} \file
 */
