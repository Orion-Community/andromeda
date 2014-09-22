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
#include <mm/cache.h>

#ifdef SLAB
struct mm_cache* vfile_cache = NULL;
struct mm_cache* vsuper_cache = NULL;
struct mm_cache* block_cache = NULL;
#endif

struct vmount* root;

static int vfs_invlCache(struct vfile* stream, uint64_t idx, size_t num);
static size_t vfs_write(struct vfile* stream, char* buf, size_t num);
static size_t vfs_read(struct vfile* stream, char* buf, size_t num);
static int vfs_seek(struct vfile* stream, int64_t idx, seek_t from);
static int vfs_close(struct vfile* stream);

static int vfs_flush(struct vfile* stream);

static int vfs_cache_dtor(void* data, void* args __attribute__((unused)))
{
        int ret = -E_SUCCESS;
#ifdef SLAB
        ret = mm_cache_free(block_cache, data);
#else
        kfree(data);
#endif
        return ret;
}

#ifdef SLAB
static void vfs_cache_block_dtor(void* data, struct mm_cache* cache __attribute__((unused)), uint32_t data_size __attribute__((unused)))
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return;
}

static void vfs_cache_block_ctor(void* data, struct mm_cache* cache __attribute__((unused)), uint32_t data_size __attribute__((unused)))
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return;
}
#endif

/**
 * \fn vfs_open_dir
 * \brief Connect the file descriptor to a directory file
 * \param file
 * \param path
 * \param strln
 * \return A generic error code
 */
static int vfs_open_dir(struct vfile* file __attribute__((unused)), char* path __attribute__((unused)), size_t strln __attribute__((unused)))
{
        printf("VFS_OPEN_DIR unimplemented!");
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
static int vfs_open(struct vfile* file, char* path, size_t strln)
{
        printf("VFS_OPEN unimplemented!");
        if (file->type == DIR)
                return vfs_open_dir(file, path, strln);
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
                return NULL ;

        memset(file, 0, sizeof(*file));

        file->open = vfs_open;
        file->read = vfs_read;
        file->write = vfs_write;
        file->close = vfs_close;
        file->seek = vfs_seek;
        file->flush = vfs_flush;
        file->invlCache = vfs_invlCache;

        file->type = FILE;
        file->cache = tree_new_avl();

        return file;
}

static int vfs_close(struct vfile* stream)
{
        if (stream == NULL || stream->close == NULL)
                return -E_NULL_PTR;
        if (stream->flush == NULL || stream->cache == NULL
                        || stream->cache->flush == NULL) {
                return -E_CORRUPT;
        }

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

/**
 * \fn vfs_get_cache_block
 * @param file
 * @param block_id
 *
 * This function will allocate a new cache block if necessary. If the file is
 * connected to a file somewhere on the file system, that responsibility is
 * delegated towards the file system.
 *
 * This function will return either what the filesystem returned or the newly
 * allocated block.
 */
static void* vfs_get_cache_block(struct vfile* file, idx_t block_id)
{
        if (file == NULL)
                return NULL ;

        /* Acquire the relevant block */
        void* data = file->cache->find(block_id, file->cache);
        /* If no block was found allocate a new one */
        if (data == NULL && (file->fs_data.read != NULL))
        {
                file->fs_data.read(file, block_id, 1);
                data = file->cache->find(block_id, file->cache);
        }
        else if (data == NULL) {
#ifdef SLAB
                data = mm_cache_alloc(block_cache, 0);
                if (data == NULL)
                        return NULL ;
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

/**
 * \fn copy_size
 * \brief determine the number of bytes to be copied in this phase
 * \param size
 * \param cursor
 * \param done
 * \return The number of bytes to be copied
 */
static inline idx_t copy_size(size_t size, idx_t cursor, idx_t done)
{
        size_t buf = size - done;
        cursor &= (CACHE_BLOCK_SIZE - 1);

        size_t block = CACHE_BLOCK_SIZE - cursor;

        return (buf < block) ? buf : block;
}

static int vfs_block_mark_dirty(struct vfile* stream, idx_t block_id)
{
        if (stream == NULL)
                return -E_INVALID_ARG;

        if (stream->dirty[stream->next_dirty_idx] == ~(idx_t)0)
        {
                if (stream->file_flags & FILE_FLAG_WRITE_THROUGH) {
                        if (stream->fs_data.write != NULL)
                                stream->fs_data.write(stream,
                                                    stream->dirty[block_id], 1);
                        else
                                return -E_CORRUPT;
                } else {
                        stream->dirty[stream->next_dirty_idx] = block_id;
                        stream->next_dirty_idx ++;
                }
        } else if (stream->fs_data.write != NULL){
                int i = 0;
                for (; i < FS_MAX_DIRTY; i++) {
                        stream->fs_data.write(stream, stream->dirty[i], 1);
                }
        } else {
                return -E_NOMEM;
        }

        return -E_SUCCESS;
}

static size_t vfs_read(struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || stream->read == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        if (stream->cache == NULL)
                return -E_CORRUPT;

        mutex_lock(&stream->file_lock);

        size_t ret = 0;

        idx_t block_id = stream->cursor >> CACHE_BLOCK_PWR;
        idx_t offset = stream->cursor & (CACHE_BLOCK_SIZE - 1);

        size_t no_blocks = num / CACHE_BLOCK_SIZE;
        idx_t i = 0;
        for (; i < no_blocks; i++)
        {
                void* block = vfs_get_cache_block(stream, block_id);
                if (block == NULL)
                {
                        ret = -E_NOTFOUND;
                        goto err;
                }

                /*idx_t blk_len2 = (num % CACHE_BLOCK_SIZE) - offset; */
                idx_t blk_len = ((num & CACHE_BLOCK_SIZE) - 1) - offset;

                memcpy (buf + i, block + offset, blk_len);
                block_id++;
                ret += blk_len;

                offset = 0;
        }

err:
        mutex_unlock(&stream->file_lock);
        return ret;
}

/**
 * \fn vfs_write
 * \param stream
 * \param buf
 * \param num
 * \return number of bytes written or negative error value
 */
static size_t vfs_write(struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        if (stream->write == NULL || stream->cache == NULL)
                return -E_NOT_YET_INITIALISED;

        /* Make sure we're the only ones writing to the stream now */
        mutex_lock(&stream->file_lock);
        /* Determine where to write */
        idx_t block_idx = stream->cursor >> CACHE_BLOCK_PWR;
        /* Get the relevant first block */
        void* block = vfs_get_cache_block(stream, block_idx);

        /* See whether we got the block */
        idx_t i = 0;
        if (block == NULL) {
                i = -E_NOMEM;
                goto err;
        }

        /* Determine how much has to be copied this block */
        idx_t overflow = stream->cursor & (CACHE_BLOCK_SIZE - 1);
        idx_t cpy_end = copy_size(num, overflow, i);

        /* First copy */
        memcpy(block + overflow, buf, cpy_end);
        if (vfs_block_mark_dirty(stream, block_idx) != -E_SUCCESS) {
                i = -E_FS_SYNC;
                goto err;
        }

        /* Repeat the above process for any remaining blocks in a loop */
        for (; i < num; i += cpy_end) {
                block_idx++;
                block = vfs_get_cache_block(stream, block_idx);
                if (block == NULL) {
                        i = -E_NOMEM;
                        goto err;
                }
                cpy_end = copy_size(num, 0, i);

                memcpy(block, buf + i, CACHE_BLOCK_SIZE - cpy_end);
                if (vfs_block_mark_dirty(stream, block_idx) != -E_SUCCESS) {
                        i = -E_FS_SYNC;
                        goto err;
                }
        }

        stream->cursor += i;

        err:
        /* Unlock */
        mutex_unlock(&stream->file_lock);
        /* Return the number of bytes written */
        return (size_t)i;
}

static int vfs_seek(struct vfile* stream, int64_t idx __attribute__ ((unused)), seek_t from __attribute__((unused)))
{
        if (stream == NULL || stream->seek == NULL)
                return -E_NULL_PTR;

        printf("VFS_SEEK unimplemented!\n");

        return -E_NOFUNCTION;
}

static int vfs_flush(struct vfile* stream)
{
        if (stream == NULL || stream->flush == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

static int vfs_invlCache(struct vfile* stream, uint64_t idx __attribute__((unused)), size_t num __attribute__((unused)))
{
        if (stream == NULL)
                return -E_INVALID_ARG;

        printf("VFS_INVLCACHE unimplemented!\n");

        return -E_NOFUNCTION;
}

int vfs_mount(struct vfile* stream, struct vdir_ent* entry)
{
        if (stream == NULL || entry == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_change_root(char* path __attribute__((unused)), int strlen __attribute__((unused)), unsigned int pid __attribute__((unused)))
{
        /* This is relevant to the current process and any processes that are
         * cloned after this function has been called */

        printf("VFS_CHANGE_ROOT unimplemented!\n");

        return -E_NOFUNCTION;
}

int vfs_init()
{
#ifdef SLAB
        size_t fsize = sizeof(struct vfile);
        size_t ssize = sizeof(struct vsuper_block);
        size_t csize = CACHE_BLOCK_SIZE;
        vfile_cache = mm_cache_init("vfile", fsize, fsize, NULL, NULL);
        vsuper_cache = mm_cache_init("vsuper", ssize, ssize, NULL, NULL);
        block_cache = mm_cache_init("vfs_blocks", csize, csize,
                        vfs_cache_block_ctor, vfs_cache_block_dtor);
#endif

        return -E_NOFUNCTION;
}
