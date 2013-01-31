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

/**
 * \AddToGroup Stream
 * @{
 */

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

        int tmp = pipe->data->flush(pipe->data, FLUSH_DEALLOC);
        if (tmp == -E_SUCCESS)
                pipe->data = NULL;
        return tmp;
}

/**
 * \fn pipe_read
 * \brief Read from pipe
 */
static int pipe_read(struct pipe* pipe, char* data, int len)
{
        if (pipe == NULL || data == NULL || len == 0)
                return -E_INVALID_ARG;

        /* Maybe do some reading bits here? */

        return -E_NOFUNCTION;
}

/**
 * \fn pipe_write
 * \brief Write to pipe
 */
static int pipe_write(struct pipe* pipe, char* data)
{
        if (pipe == NULL || data == NULL)
                return -E_NULL_PTR;

        int key = pipe->writing_idx / pipe->block_size;
        int offset = pipe->writing_idx % pipe->block_size;

        struct tree* block = pipe->data->find(key, pipe->data);

        /* Now do some writing bits ... */

        return -E_NOFUNCTION;
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
                kfree(pipe);

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
        struct pipe* p = kalloc(sizeof(*p));
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
