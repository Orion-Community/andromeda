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
#include <andromeda/system.h>

static idx_t
get_idx(idx_t offset, int depth)
{
        int mul = BUFFER_TREE_DEPTH-(int)depth;
        int shift = mul*BUFFER_OFFSET_BITS;
        idx_t ret = (offset>>shift)&(BUFFER_LIST_SIZE-1);
        return ret;
}

static int
buffer_init_branch(struct buffer_list* this, struct buffer* parent)
{
        if (this == NULL)
                return -E_NULL_PTR;

        memset(this, 0, sizeof(struct buffer_list));
        this->parent = parent;
        return -E_SUCCESS;
}

/**
 * \fn buffer_rm_block
 * \brief Removes a block from the buffer and the related branches if empty
 *
 * \param this The buffer to work with
 * \param offset The index at which the block resides
 * \param depth For external calls, keep 0
 */

static int
buffer_rm_block(struct buffer_list* this, idx_t offset, int depth)
{
        if (depth > BUFFER_TREE_DEPTH)
                return -E_INVALID_ARG;

        if (this == NULL)
                goto err;

        idx_t idx = get_idx(offset, depth);

        mutex_lock(&this->lock);

        int ret = -E_SUCCESS;
        if (depth != BUFFER_TREE_DEPTH)
        {
                struct buffer_list* list = this->lists[idx];
                if (list == NULL)
                        goto err_locked;
                ret = buffer_rm_block(list, offset, depth+1);
                switch(ret)
                {
                case -E_CLEAN_PARENT:
                        kfree(this->lists[idx]);
                        this->lists[idx] = NULL;
                        mutex_unlock(&this->lock);
                        if (atomic_dec(&this->used) == 0)
                                return -E_CLEAN_PARENT;
                        return -E_SUCCESS;
                default:
                        mutex_unlock(&this->lock);
                        return ret;
                }
        }

        if (this->blocks[idx] == NULL)
                goto err_locked;
        kfree(this->blocks[idx]);
        this->blocks[idx] = NULL;

        mutex_unlock(&this->lock);
        if (atomic_dec(&this->used) == 0)
                return -E_CLEAN_PARENT;

        return -E_SUCCESS;

err_locked:
        mutex_unlock(&this->lock);
err:
        return -E_NULL_PTR;
}

/**
 * \fn buffer_clean_up
 * \brief Remove all data from 0 up to base_idx
 *
 * \param this The buffer to clean up
 */

static int
buffer_clean_up(struct buffer* this)
{
        if (this == NULL)
                return -E_NULL_PTR;

        /** Clean up from the location last cleaned untill base_idx */

        idx_t idx = this->cleaned;
        for (; idx < (this->base_idx/BUFFER_BLOCK_SIZE); idx++)
                buffer_rm_block(this->blocks, idx, 0);

        this->cleaned = idx;

        return -E_SUCCESS;
}

/**
 * \fn buffer_add_block
 * \brief Add a block to a certain offset into the buffer
 *
 * \param this The buffer to place the new block into
 * \param list The list to add the block to
 * \param offset Where we should place the new block
 * \param depth for external calls, keep 0
 */

static int
buffer_add_block(this, list, offset, depth)
struct buffer_block* this;
struct buffer_list* list;
idx_t offset;
int depth;
{
        if (list == NULL)
                return -E_NULL_PTR;
        if (offset > list->parent->size/BUFFER_BLOCK_SIZE)
                return -E_OUTOFBOUNDS;

        int ret = 0;
        idx_t list_idx = get_idx(offset, depth);

        mutex_lock(&list->lock);

        if (depth != BUFFER_TREE_DEPTH)
        {
                if (list->lists[list_idx] == NULL)
                {
                        list->lists[list_idx] = kmalloc(
                                                sizeof(*list->lists[list_idx]));
                        if (list->lists[list_idx] == NULL)
                        {
                                mutex_unlock(&list->lock);
                                return -E_NULL_PTR;
                        }
                        buffer_init_branch(list->lists[list_idx], list->parent);
                        atomic_inc(&(list->used));
                }
                ret = buffer_add_block(this, list->lists[list_idx],
                                                                         offset,
                                                                       depth+1);
                mutex_unlock(&list->lock);
                return ret;
        }

        if (list->blocks[list_idx] != NULL)
        {
                mutex_unlock(&list->lock);
                return -E_ALREADY_INITIALISED;
        }

        atomic_inc(&(list->used));
        list->blocks[list_idx] = this;
        mutex_unlock(&list->lock);
        return -E_SUCCESS;
}

/**
 * \fn buffer_find_block
 * \brief Helper function to find the requested block_id
 *
 * \param this The buffer to seek in
 * \param offset The block offset in the buffer
 * \param depth For external calls, keep 0
 */

static struct buffer_block*
buffer_find_block(struct buffer_list* this, idx_t offset, int depth)
{
        if (this == NULL)
                goto err;

        idx_t list_idx = get_idx(offset, depth);
        struct buffer_block* ret;

        if (depth != BUFFER_TREE_DEPTH)
                ret = buffer_find_block(this->lists[list_idx], offset, depth+1);
        else
        {
                ret = this->blocks[list_idx];
        }
        return ret;

err:
        return NULL;
}

/**
 * \fn buffer_write
 * \brief Write data to stream
 *
 * \param this The stream to write to
 * \param buf The char array to write to the stream
 * \param num The size of the char array
 */

static size_t
buffer_write(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL)
                return 0;

        if (this->cursor < ((struct buffer*)(this->fs_data))->base_idx)
                this->cursor = ((struct buffer*)(this->fs_data))->base_idx;

        idx_t offset = this->cursor / BUFFER_BLOCK_SIZE;
        idx_t block_cur = this->cursor % BUFFER_BLOCK_SIZE;

        struct buffer* buffer = this->fs_data;
        if (buffer == NULL)
                return 0;

        if (buffer->blocks == NULL)
        {
                buffer->blocks = kmalloc(sizeof(*buffer->blocks));
                if (buffer->blocks == NULL)
                        return 0;
                buffer_init_branch(buffer->blocks ,buffer);
        }
        struct buffer_block* b = buffer_find_block(buffer->blocks, offset, 0);
        if (b == NULL)
        {
                struct buffer_block* block = kmalloc(sizeof(*block));
                buffer_add_block(block, buffer->blocks, offset, 0);
                b = buffer_find_block(buffer->blocks, offset, 0);
                if (b == NULL)
                        return 0;
        }

        if (this->cursor+num > buffer->size &&
                                        !(buffer->rights & BUFFER_ALLOW_GROWTH))
                num = buffer->size - this->cursor;

        size_t idx = 0;

        for (; idx < num; idx++, block_cur++)
        {
                if (block_cur >= BUFFER_BLOCK_SIZE)
                {
                        offset++;
                        block_cur -= BUFFER_BLOCK_SIZE;
                        b = buffer_find_block(buffer->blocks, offset, 0);
                        if (b == NULL)
                        {
                                buffer_add_block(kmalloc(
                                                   sizeof(struct buffer_block)),
                                                                 buffer->blocks,
                                                                     offset, 0);
                                b = buffer_find_block(buffer->blocks,offset, 0);
                                if (b == NULL)
                                {
                                        this->cursor += idx;
                                        return idx;
                                }
                        }
                }
                b->data[block_cur] = buf[idx];
        }

        this->cursor += idx;

        return idx;
}

/**
 * \fn buffer_read
 * \brief get data from stream
 *
 * \param this The stream to read from
 * \param buf The char array to write to from the stream
 * \param num The size of the char array
 */

static size_t
buffer_read(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL)
                return 0;

        if (this->cursor < ((struct buffer*)(this->fs_data))->base_idx)
                this->cursor = ((struct buffer*)(this->fs_data))->base_idx;

        idx_t offset = this->cursor / BUFFER_BLOCK_SIZE;
        idx_t block_cur = this->cursor % BUFFER_BLOCK_SIZE;

        struct buffer* buffer = this->fs_data;
        if (buffer == NULL)
                return 0;

        struct buffer_block* b = buffer_find_block(buffer->blocks, offset, 0);
        if (b == NULL)
                return 0;

        if (this->cursor+num > buffer->size)
                num = buffer->size - this->cursor;

        size_t idx = 0;
        for (; idx < num; idx++, block_cur++)
        {
                if (block_cur >= BUFFER_BLOCK_SIZE)
                {
                        offset++;
                        block_cur -= BUFFER_BLOCK_SIZE;
                        b = buffer_find_block(buffer->blocks, offset, 0);
                        if (b == NULL)
                        {
                                if (this->cursor + idx < ((struct buffer*)
                                                         (this->fs_data))->size)
                                {
                                        buf[idx] = '\0';
                                        continue;
                                }
                                this->cursor += idx;
                                return idx;
                        }
                }
                buf[idx] = b->data[block_cur];
        }

        this->cursor += num;
        return num;
}

/**
 * \fn buffer_seek
 *
 * \brief seek in the file
 *
 * \param this a pointer to the file we're working with.
 * \param offset what's the distance from the "from" indicator.
 * \param from what's the point we have to seek from.
 */

static int
buffer_seek(struct vfile* this, int64_t offset, seek_t from)
{
        if (this == NULL || this->fs_data == NULL)
                return -E_NULL_PTR;

        struct buffer* buf = this->fs_data;
        switch(from)
        {
        case SEEK_SET:
                if (offset < buf->base_idx)
                        this->cursor = buf->base_idx;
                else if (offset > buf->size)
                        this->cursor = buf->size;
                else
                        this->cursor = offset;
                break;

        case SEEK_CUR:
                if (offset <= 0)
                {
                        if (-offset > this->cursor - buf->base_idx)
                        {
                                this->cursor = buf->base_idx;
                        }
                        else
                        {
                                this->cursor += offset;
                        }
                }
                else
                {
                        if (offset > buf->size - this->cursor)
                        {
                                this->cursor = buf->size;
                        }
                        else
                        {
                                this->cursor += offset;
                        }
                }
                break;

        case SEEK_END:
                if (offset > 0)
                        this->cursor = buf->size;
                else if (offset < 0 && -offset < (buf->size - buf->base_idx))
                        this->cursor += offset;
                else
                        this->cursor = buf->base_idx;
                break;

        default:
                debug("Buffer seek doesn't support mode: %X\n", from);
                return -E_INVALID_ARG;
                break;
        }
        return -E_SUCCESS;
}

/**
 * \fn buffer_close
 * \brief closes the buffer, and cleans up the data if it's the last buffer
 * \brief standing.
 *
 * \param this the buffer to close
 */

static int
buffer_close(struct vfile* this)
{
        if (this == NULL || this->fs_data == NULL)
                return -E_NULL_PTR;

        struct buffer* buf = (struct buffer*)this->fs_data;

        if (atomic_dec(&(buf->opened)) == 0)
        {
                /** clean up the entire buffer */
                buf->base_idx = buf->size;
                int ret = buffer_clean_up(this->fs_data);
                kfree(buf->blocks);
                kfree(buf);
                kfree(this);
                if (ret == -E_CLEAN_PARENT)
                        return -E_SUCCESS;
                return -E_GENERIC;
        };
        kfree(this);
        /** we have removed this instance by running atomic_dec  */
        return -E_SUCCESS;
}

/**
 * \fn buffer_duplicate
 * \brief takes only one argument, which is the buffer to duplicate. It returns
 * \brief the duplicated buffer.
 *
 * \param this the buffer to duplicate
 */

static struct vfile*
buffer_duplicate(struct buffer *this)
{
        if (!(this->rights & (BUFFER_ALLOW_DUPLICATE)))
                return NULL;

        struct vfile* file = kmalloc(sizeof(*file));
        if (file == NULL)
                return NULL;
        memset(file, 0, sizeof(*file));

        file->close = buffer_close;
        file->write = buffer_write;
        file->read = buffer_read;
        file->seek = buffer_seek;

        file->fs_data = this;
        file->fs_data_size = sizeof(*this);

        atomic_inc(&(this->opened));
        return file;
}

/**
 * \fn buffer_init
 * \brief Initialise a new buffer
 * takes 2 arguments:
 *
 * \param size, sets the size of the buffer.
 * \brief size in buffer blocks, when nog BUFFER_DYNAMIC_SIZE
 * \brief if BUFFER_DYNAMIC_SIZE the buffer will grow on write, if necessary
 * \param base_idx, tells us up to which point we're allowed to clean up.
 *      From 0 untill base_idx, nothing will be written.
 *      It will also set the standard cursor.
 *
 * This function returns the newly created buffer.
 */

int
buffer_init(struct vfile* this, idx_t size, idx_t base_idx)
{
        if (this == NULL)
                return -E_NULL_PTR;

        struct buffer* b = kmalloc(sizeof(*b));
        if (b == NULL)
                return -E_NOMEM;
        memset(b, 0, sizeof(*b));

        this->read = buffer_read;
        this->seek = buffer_seek;
        this->write = buffer_write;
        this->close = buffer_close;

        b->duplicate = buffer_duplicate;
        b->size = size*BUFFER_BLOCK_SIZE;
        b->base_idx = base_idx;
        b->rights |= (size == BUFFER_DYNAMIC_SIZE) ? BUFFER_ALLOW_GROWTH : 0;
        b->rights |= BUFFER_ALLOW_DUPLICATE;

        atomic_inc(&b->opened);

        this->fs_data = b;
        this->fs_data_size = sizeof(*b);
        return -E_SUCCESS;
}
