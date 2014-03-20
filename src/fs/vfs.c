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

#include <fs/vfs.h>
#include <andromeda/system.h>

#ifdef SLAB
struct mm_cache* vfile_cache = NULL;
struct mm_cache* vsuper_cache = NULL;
struct mm_cache* block_cache = NULL;
#endif

struct vmount* root;

static int vfs_flush (struct vfile* stream);

static int
vfs_cache_dtor(void* data, void* args)
{
        int ret = -E_SUCCESS;
#ifdef SLAB
        ret = mm_cache_free(block_cache, data);
#else
        ret = kfree(data);
#endif
        return ret;
}

int vfs_cache_block_dtor(void* data, struct mm_cache* cache, void* args)
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return -E_SUCCESS;
}

int vfs_cache_block_ctor(void* data, struct mm_cache* cache, void* args)
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return -E_SUCCESS;
}

/**
 * \fn vfs_open_dir
 * \brief Connect the file descriptor to a directory file
 * \param file
 * \param path
 * \param strln
 * \return A generic error code
 */
int vfs_open_dir (struct vfile* file, char* path, size_t strln)
{
        return -E_NOFUNCTION;
}
/**
 * \fn vfs_open
 * \brief Connect a file descriptor to a file somewhere on the file system
 * \param file
 * \param path
 * \param strln
 * \return A generic error code
 */
int vfs_open (struct vfile* file, char* path, size_t strln)
{

        return -E_NOFUNCTION;
}

/**
 * \fn vfs_create
 * \brief Create a new file, this has not been coupled to a file on the fs
 * \return The newly created file descriptor
 */
struct vfile*
vfs_create()
{
        struct vfile* file;
#ifndef SLAB
        file = kmalloc(sizeof(*file));
#else
        file = mm_cache_alloc(vfile_cache, 0);
#endif
        if (file == NULL)
                return NULL;

        memset(file, 0, sizeof(*file));

        file->open = vfs_open;
        file->read = vfs_read;
        file->write = vfs_write;
        file->close = vfs_close;
        file->seek = vfs_seek;
        file->flush = vfs_flush;

        file->type = FILE;
        file->cache = tree_new_avl();

        return file;
}

int vfs_close (struct vfile* stream)
{
        if (stream == NULL || stream->close == NULL)
                return -E_NULL_PTR;

        int ret = stream->flush(stream);

        ret |= stream->cache->flush(stream->cache, vfs_cache_dtor, NULL);

        if (ret != -E_SUCCESS)
                return ret;

#ifdef SLAB
        mm_cache_free(vfile_cache, stream);
#else
        kfree(stream);
#endif
        return -E_SUCCESS;
}

int vfs_read  (struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || stream->read == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;



        return -E_NOFUNCTION;
}

static void* vfs_get_cache_block(struct vfile* file, idx_t block_id)
{
        if (file == NULL)
                return NULL;

        void* data = file->cache->find(block_id, file->cache);
        if (data == NULL)
        {
#ifdef SLAB
                data = mm_cache_alloc(block_cache, 0);
                if (data == NULL)
                        return NULL;
#else
                data = kmalloc(CACHE_BLOCK_SIZE);
                if (data == NULL)
                        return NULL;
                memset(data, 0, CACHE_BLOCK_SIZE);
#endif

                file->cache->add(block_id, data, file->cache);
        }

        return data;
}

static inline idx_t copy_size(size_t size, idx_t cursor, idx_t done)
{
        size_t buf = size - done;
        cursor &= (CACHE_BLOCK_SIZE - 1);

        size_t block = CACHE_BLOCK_SIZE - cursor;

        return (buf < block) ? buf : block;
}

int vfs_write (struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        if (stream->write == NULL || stream->cache == NULL)
                return -E_NOT_YET_INITIALISED;

        mutex_lock(stream->file_lock);
        idx_t block_idx = stream->cursor >> CACHE_BLOCK_PWR;
        void* block = vfs_get_cache_block(stream, block_idx);

        idx_t i = 0;
        if(block == NULL)
        {
                i = -E_NOMEM;
                goto err;
        }

        idx_t overflow = stream->cursor & (CACHE_BLOCK_SIZE - 1);
        idx_t cpy_end = copy_size(num, overflow, i);

        memcpy(block + overflow, buf, cpy_end);

        for (; i < num; i += cpy_end)
        {
                block_idx++;
                block = vfs_get_cache_block(stream, block_idx);
                if (block == NULL)
                {
                        i = -E_NOMEM;
                        goto err;
                }
                cpy_end = copy_size(num, 0, i);

                memcpy(block, buf + i, CACHE_BLOCK_SIZE - cpy_end);
        }

err:
        mutex_unlock(stream->file_lock);
        return i;
}

int vfs_seek (struct vfile* stream, size_t idx, seek_t from)
{
        if (stream == NULL || stream->seek == NULL)
                return -E_NULL_PTR;

        stream->seek(stream, idx, from);

        return -E_NOFUNCTION;
}

static int vfs_flush (struct vfile* stream)
{
        if (stream == NULL || stream->flush == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_mount(struct vfile* stream, struct vdir_ent* entry)
{
        if (stream == NULL || entry == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_change_root(char* path, int strlen, unsigned int pid)
{
        /* This is relevant to the current process and any processes that are
         * cloned after this function has been called */
        return -E_NOFUNCTION;
}

int vfs_init()
{
#ifdef SLAB
        size_t fsize = sizeof(struct vfile);
        size_t ssize = sizeof(struct vsuper_block);
        size_t csize = CACHE_BLOCK_SIZE;
        vfile_cache  = mm_cache_init("vfile", fsize, 0, NULL, NULL);
        vsuper_cache = mm_cache_init("vsuper", ssize, 0, NULL, NULL);
        block_cache = mm_cache_init("vfs_blocks", csize, 0,
                        vfs_cache_block_ctor, vfs_cache_block_dtor);
#endif

        return -E_NOFUNCTION;
}
