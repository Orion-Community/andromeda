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

static size_t vfs_write(struct vfile* stream, char* buf, size_t num);
static size_t vfs_read(struct vfile* stream, char* buf, size_t num);
static int vfs_close(struct vfile* stream);

static int vfs_flush(struct vfile* stream);

#ifdef SLAB
static void vfs_cache_block_dtor(void* data,
                struct mm_cache* cache __attribute__((unused)),
                uint32_t data_size __attribute__((unused)))
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return;
}

static void vfs_cache_block_ctor(void* data,
                struct mm_cache* cache __attribute__((unused)),
                uint32_t data_size __attribute__((unused)))
{
        memset(data, 0, CACHE_BLOCK_SIZE);
        return;
}
#endif

static int vfs_sync(struct vfile* this)
{
        if (this == NULL) {
                return -E_NULL_PTR;
        }

        if (this->out_stream != NULL) {
                this->out_stream->sync_write(this->out_stream);
        }
        if (this->in_stream != NULL) {
                this->in_stream->sync_read(this->in_stream);
        }

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
static int vfs_open_dir(struct vfile* file __attribute__((unused)),
                char* path __attribute__((unused)),
                size_t strln __attribute__((unused)))
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

static size_t fs_write_dummy(struct vfile* file __attribute__((unused)),
                char* buf __attribute__((unused)),
                size_t offset __attribute__((unused)),
                size_t len __attribute__((unused)))
{
        return 0;
}

static size_t fs_read_dummy(struct vfile* file __attribute__((unused)),
                char* buf __attribute__((unused)),
                size_t offset __attribute__((unused)),
                size_t len __attribute__((unused)))
{
        return 0;
}

static int fs_close_dummy(struct vfile* file __attribute__((unused)))
{
        return 0;
}

static int fs_open_dummy(struct vfile* file __attribute__((unused)),
                char* path __attribute__((unused)),
                size_t len __attribute__((unused)))
{
        return 0;
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
        if (file == NULL) {
                warning("File allocation failed!\n");
                return NULL ;
        }

        memset(file, 0, sizeof(*file));

        file->in_stream = pipe_new(file, file);
        if (file->in_stream == NULL) {
                warning("Pipe failed!\n");
                goto err;
        }
        file->out_stream = file->in_stream;

        file->open = vfs_open;
        file->read = vfs_read;
        file->write = vfs_write;
        file->close = vfs_close;
        file->flush = vfs_flush;
        file->sync = vfs_sync;
        file->fs_data.read = fs_read_dummy;
        file->fs_data.write = fs_write_dummy;
        file->fs_data.open = fs_open_dummy;
        file->fs_data.close = fs_close_dummy;

        file->type = FILE;

        return file;

        err:
#ifdef SLAB
        mm_cache_free(vfile_cache, file);
#else
        kfree(file);
#endif
        return NULL ;
}

static int vfs_close(struct vfile* stream)
{
        if (stream == NULL || stream->close == NULL)
                return -E_NULL_PTR;

        int ret = stream->flush(stream);
        if (ret != -E_SUCCESS) {
                /* Don't actually close yet, we might have removed necessary
                 * data, but not all pointers have been freed by the cleanup.
                 *
                 * Do write a warning, but keep the pointers, for future cleanup
                 */
                warning("Failed to close a file, memory lingering!\n");
                return ret;
        }

#ifdef SLAB
        mm_cache_free(vfile_cache, stream);
#else
        kfree(stream);
#endif
        return -E_SUCCESS;
}

static size_t vfs_read(struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || buf == NULL || num == 0) {
                return 0;
        }
        if (stream->in_stream == NULL || stream->in_stream->read == NULL) {
                return 0;
        }

        size_t ret = 0;

        ret = stream->in_stream->read(stream->in_stream, buf, num);

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
        if (stream == NULL || buf == NULL || num == 0) {
                return 0;
        }
        if (stream->out_stream == NULL || stream->out_stream->write == NULL) {
                return 0;
        }

        size_t ret = stream->out_stream->write(stream->out_stream, buf, num);

        /* Return the number of bytes written */
        return ret;
}

static int vfs_flush(struct vfile* stream)
{
        if (stream == NULL || stream->flush == NULL)
                return -E_NULL_PTR;

        int ret = -E_SUCCESS;
        if (stream->in_stream != NULL) {
                ret |= stream->in_stream->purge(stream->in_stream);
        }
        if (stream->out_stream != NULL) {
                ret |= stream->out_stream->purge(stream->out_stream);
        }

        return ret;
}

int vfs_mount(struct vfile* stream, struct vdir_ent* entry)
{
        if (stream == NULL || entry == NULL)
                return -E_NULL_PTR;

        warning("Mounting filesystems not yet supported!\n");

        return -E_NOFUNCTION;
}

int vfs_change_root(char* path __attribute__((unused)),
                int strlen __attribute__((unused)),
                unsigned int pid __attribute__((unused)))
{
        /* This is relevant to the current process and any processes that are
         * cloned after this function has been called */

        warning("vfs_change_root unimplemented!\n");

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
