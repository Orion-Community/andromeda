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

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/buffer.h>

#define BUFFER_BLOCK_IDX(a) (a-(a%BUFFER_BLOCK_SIZE))

/**
 * buffer_add_branch adds a branch to the buffer block tree
 */
static int
buffer_add_branch(struct buffer_list* this, idx_t idx, buffer_list_t type)
{
        if (this == NULL)
                return -E_NULL_PTR;

        if (this->type == blocks || idx > BUFFER_LIST_SIZE)
                return -E_INVALID_ARG;

        mutex_lock(this->lock);
        if (this->lists[idx] != NULL)
        {
                mutex_unlock(this->lock);
                return -E_ALREADY_INITIALISED;
        }

        struct buffer_list* to_add = kalloc(sizeof(struct buffer_list));
        if (to_add == NULL)
                return -E_NOMEM;
        memset(to_add, 0, sizeof(struct buffer_list));

        to_add->type = type;

        this->lists[idx] = to_add;
        atomic_inc(&(this->used));

        mutex_unlock(this->lock);
        return -E_SUCCESS;
}

/**
 * buffer_rm_block removes a block from the buffer and the related branches if
 * empty
 */

static int
buffer_rm_block(struct buffer* this, idx_t offset)
{
        offset = BUFFER_BLOCK_IDX(offset);
        warning("buffer_rm_block not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_clean_direct(struct buffer* this, idx_t block_id)
{
        return -E_NOFUNCTION;
}

#define CLEAN_LIST 0x1

static int
buffer_clean_indirect(struct buffer_list* list, idx_t block_id, idx_t level)
{
        idx_t list_idx = block_id - BUFFER_LIST_SIZE;
        if (list == NULL)
                return -E_NULL_PTR;
        mutex_lock(list->lock);
        switch (level)
        {
        case 3:
                list_idx /= BUFFER_LIST_SIZE;
        case 2:
                list_idx /= BUFFER_LIST_SIZE;
        case 1:
                list_idx &= 0x3FF;
                break;
        default:
                mutex_unlock(list->lock);
                return -E_INVALID_ARG;
        }
        if (level == 1)
        {
                if (list->blocks[list_idx] != NULL)
                {
                        mutex_lock(list->lock);
                        free(list->blocks[list_idx]);
                        list->blocks[list_idx] = 0;
                        atomic_dec(&(list->used));

                        if (atomic_get(&(list->used)) == 0)
                        {
                                mutex_unlock(list->lock);
                                return CLEAN_LIST;
                        }

                        mutex_unlock(list->lock);
                }
        }
        else
        {
                switch (buffer_clean_indirect(list->lists[list_idx], block_id,
                                                                       level-1))
                {
                case -E_SUCCESS:
                        mutex_unlock(list->lock);
                case CLEAN_LIST:
                        free(list->lists[list_idx]);
                        list->lists[list_idx] = NULL;
                        if (atomic_dec(&(list->used)) == 0)
                        {
                                mutex_unlock(list->lock);
                                return CLEAN_LIST;
                        }
                        break;
                case -E_INVALID_ARG:
                        mutex_unlock(list->lock);
                        return -E_INVALID_ARG;
                case -E_NULL_PTR:
                        mutex_unlock(list->lock);
                        return -E_NULL_PTR;
                case -E_GENERIC:
                        mutex_unlock(list->lock);
                        return -E_GENERIC;
                default:
                        mutex_unlock(list->lock);
                        return -E_GENERIC;
                }
        }
        return -E_SUCCESS;
}

static int
buffer_clean_up(struct buffer* this)
{
        warning("buffer_clean_up not yet implemented");

        /** Clean up untill base_idx == size */



        return -E_NOFUNCTION;
}

static int
buffer_add_block(struct buffer* this, idx_t offset)
{
        offset = BUFFER_BLOCK_IDX(offset);
        warning("buffer_add_block not yet implemented");
        return -E_NOFUNCTION;
}

static struct buffer_block*
buffer_find_block(struct buffer* this, idx_t offset)
{
        offset = BUFFER_BLOCK_IDX(offset);
        warning("buffer_find_block not yet implemented");
        return NULL;
}

static int
buffer_write(struct buffer* this, char* buf, size_t num)
{
        warning("buffer_write not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_read(struct buffer* this, char* buf, size_t num)
{
        warning("buffer_read not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_seek(struct buffer* this, long offset, seek_t from)
{
        switch(from)
        {
        case SEEK_SET:
                if (offset < this->base_idx)
                        this->cursor = this->base_idx;
                else if (offset > this->size)
                        this->cursor = this->size;
                else
                        this->cursor = offset;
                break;

        case SEEK_CUR:
                if (offset < 0 && (-offset < (this->cursor - this->base_idx)))
                        this->cursor += offset;
                else if (offset < 0)
                        this->cursor = this->base_idx;
                else if (offset > (this->size-this->cursor))
                        this->cursor = this->size;
                else
                        this->cursor += offset;
                break;

        case SEEK_END:
                if (offset > 0)
                        this->cursor = this->size;
                else if (offset < 0 && -offset < (this->size - this->base_idx))
                        this->cursor += offset;
                else
                        this->cursor = this->base_idx;
                break;

        default:
                debug("Buffer seek doesn't support mode: %X\n", from);
                return -E_INVALID_ARG;
                break;
        }
        return -E_SUCCESS;
}

static int
buffer_close(struct buffer* this)
{
        if (atomic_dec(&(this->opened)) == 0)
        {
                /** clean up the entire buffer */
                return buffer_clean_up(this);
        };
        /** we have removed this instance ...  */
        return -E_SUCCESS;
}

static int
buffer_close_ro(struct buffer* this)
{
        int ret = this->read_only_access->close(this->read_only_access);
        free(this);
        return ret;
}

static int
buffer_write_ro(struct buffer* this, char* buf, size_t num)
{
        return -E_NORIGHTS;
}

static int
buffer_read_ro(struct buffer* this, char* buf, size_t num)
{
        return -E_NOFUNCTION;
}

static int
buffer_seek_ro(struct buffer* this, long offset, seek_t from)
{
        return -E_NOFUNCTION;
}

/**
 * buffer_duplicate takes only one argument, which is the buffer to duplicate.
 * It returns the duplicated buffer.
 *
 * If the buffer will be copied to a read only structure when the
 * BUFFER_DUPLICATE_RO flag has been set in the rights.
 */

static struct buffer*
buffer_duplicate(struct buffer *this)
{
        struct buffer* b = this;
        if (!(this->rights & (BUFFER_ALLOW_DUPLICATE)))
                return NULL;

        if (!(this->rights & (BUFFER_DUPLICATE_RO)))
        {
                warning("Opening a read only buffer, not yet implemented!\n");
                struct buffer* b = kalloc(sizeof(struct buffer));
                if (b == NULL)
                        return NULL;

                memcpy (b, this, sizeof(struct buffer));
                b->rights ^= ~((uint32_t)0&BUFFER_ALLOW_WRITE);
                b->read_only_access = this;
                b->close = buffer_close_ro;
                b->write = buffer_write_ro;
                b->read = buffer_read_ro;
                b->seek = buffer_seek_ro;
        }
        atomic_inc(&(this->opened));
        return b;
}

/**
 * buffer_init takes 2 arguments:
 *
 * size, sets the size of the buffer.
 *      If size == BUFFER_DYNAMIC_SIZE, the size will be set to 0 and the buffer
 *      now is allowed to grow dynamically.
 *
 * base_idx, tells us up to which point we're allowed to clean up the buffer.
 *      From 0 untill base_idx, nothing will be written.
 *      It will also set the standard cursor.
 *
 * This function returns the newly created buffer.
 */

struct buffer*
buffer_init(idx_t size, idx_t base_idx)
{
        struct buffer* b = kalloc(sizeof(struct buffer));
        if (b == NULL)
                return NULL;
        memset(b, 0, sizeof(struct buffer));

        b->read = buffer_read;
        b->seek = buffer_seek;
        b->write = buffer_write;
        b->close = buffer_close;

        b->size = (size == BUFFER_DYNAMIC_SIZE) ? 0 : size;
        b->base_idx = base_idx;
        b->rights |= (size == BUFFER_DYNAMIC_SIZE) ? BUFFER_ALLOW_GROWTH : 0;

        b->cursor = b->base_idx;

        atomic_inc(&b->opened);

        return b;
}