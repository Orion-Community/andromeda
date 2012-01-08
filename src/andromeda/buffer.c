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

#define BUFFER_BLOCK_IDX(a) (a%BUFFER_BLOCK_SIZE)

static int
buffer_add_branch(struct buffer_list* this, idx_t idx, buffer_list_t type)
{
        if (this->type == blocks && type == lists)
                return -E_INVALID_ARG;

        struct buffer_list* to_add = kalloc(sizeof(struct buffer_list));
        if (to_add == NULL)
                return -E_NOMEM;
        memset(to_add, 0, sizeof(struct buffer_list));

        to_add->type = type;
}

static int
buffer_rm_block(struct buffer* this, idx_t offset)
{
        offset = BUFFER_BLOCK_IDX(offset);
        warning("buffer_rm_block not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_clean_up(struct buffer* this)
{
        warning("buffer_clean_up not yet implemented");
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
buffer_write(struct buffer* this, char* buf, size_t num, idx_t offset)
{
        warning("buffer_write not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_read(struct buffer* this, char* buf, size_t num, idx_t offset)
{
        warning("buffer_read not yet implemented");
        return -E_NOFUNCTION;
}

static int
buffer_close(struct buffer* this)
{
        warning("buffer_close not yet implemented");
        if (atomic_dec(&(this->opened)) == 0)
        {
                /** clean up the entire buffer */
                return buffer_clean_up(this);
        };
        /** we have removed this instance ...  */
        return -E_NOFUNCTION;
}

static struct buffer*
buffer_duplicate(struct buffer *this)
{
        warning("Buffer duplication not yet implemented");
        atomic_inc(&(this->opened));
        return this;
}

struct buffer*
buffer_init()
{
        struct buffer* b = kalloc(sizeof(struct buffer));
        if (b == NULL)
                return NULL;
        memset(b, 0, sizeof(struct buffer));

        b->read = buffer_read;
        b->write = buffer_write;
        b->close = buffer_close;

        atomic_inc(&b->opened);

        return b;
}