/*
 *  Andromeda
 *  Copyright (C) 2011 - 2105  Bart Kuivenhoven
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

#ifndef __FS_VFS_H
#define __FS_VFS_H

#include <stdlib.h>
#include <lib/tree.h>
#include <thread.h>
#include <fs/pipe.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SLAB
struct mm_cache* vsuper_cache;
#endif

#define KERN_STDIO 0x0

#define DIR_LIST_SIZE 0xFF

#define CACHE_BLOCK_PWR 10

#define CACHE_BLOCK_SIZE (1 << CACHE_BLOCK_PWR)

#define FILE_FLAG_WRITE_THROUGH (1 << 0)

#define VFIO(fn, arg1, arg2, arg3) \
static size_t fn(struct vfile* arg1, char* arg2, size_t arg3); \
static size_t fn(struct vfile* arg1, char* arg2, size_t arg3)

struct vfile;

typedef size_t (*vfs_read_hook_t)(struct vfile* file, char* buf, size_t len);
typedef size_t (*vfs_write_hook_t)(struct vfile* file, char* buf, size_t len);

typedef enum {
        SEEK_SET, SEEK_CUR, SEEK_END
} seek_t;
typedef enum {
        DIR, BLOCK_DEV, CHAR_DEV, FILE
} file_type_t;

struct vsuper_block;

#define FS_TYPE_LEN 128

struct fs_data {
        int32_t device_id;
        int32_t file_id;
        char fs_type[FS_TYPE_LEN];

        void* fs_data_struct;
        size_t fs_data_size;

        int (*open)(struct vfile* this, char* path, size_t strlen);
        int (*close)(struct vfile* this);
        fs_read_hook_t read;
        fs_write_hook_t write;
};

/** \struct vfile */
/** \brief What we're looking at now is the file descriptor */
struct vfile {
        /**
         * \var uid
         * \brief user_id
         * \var gid
         * \brief group_id
         * \var rights
         */
        uint32_t uid;
        uint32_t gid;
        uint16_t rights;

        file_type_t type;

        uint32_t file_flags;

        mutex_t file_lock;

        /**
         * \var dir_ent
         * \brief Parent directory entry
         * \var super
         * \brief The super block to which we're related
         * \var fs_data_size
         * \var fs_data
         * \brief The data related to this file by the fs driver
         */
        struct vdir_ent *dir_ent;
        struct vsuper_block *super;

        struct pipe* in_stream;
        struct pipe* out_stream;

        /**
         * \fn open(this, path, strlen)
         * \brief Connect file to URI
         * \fn close (this)
         * \brief Close file, flushes automatically
         * \fn read(this, buf, num)
         * \brief Read from cache if possible, from source otherwise
         * \fn write(this, buf, num)
         * \brief Write to cache
         * \fn flush(this)
         * \brief Write cache to disc
         * \fn invlCache(this, idx, num)
         * \brief Invalidate cache from idx to idx+num
         * \fn sync(this)
         * \brief Synchronize the streams to whatever location, if possible
         */
        int (*open)(struct vfile* this, char* path, size_t strlen);
        int (*close)(struct vfile* this);
        size_t (*read)(struct vfile* this, char* buf, size_t num);
        size_t (*write)(struct vfile* this, char* buf, size_t num);
        int (*flush)(struct vfile* this);
        int (*invlCache)(struct vfile* this, uint64_t idx, size_t num);
        int (*sync)(struct vfile* this);
        int (*ioctl)(struct vfile* this, uint32_t request, void* data);

        struct fs_data fs_data;
};

/** \struct vdir_ent */
/** \brief Hello directory entry. */
struct vdir_ent {
        struct vsuper_block* super;
        struct vfile* data;
        char *name;
};

/** \struct vdir */
struct vdir {
        struct vdir_ent* entries[DIR_LIST_SIZE];
        struct vdir* next;
        struct vsuper_block* mounted;
};

/** \struct vsuper_block */
/** \brief The file system descriptor. */
struct vsuper_block {
        struct vfile* fs_root;
        struct vfile* dev;

        size_t file_name_size;
        size_t fs_data_size;
        void* fs_data;

        struct vmount* mounts;

        struct vfile* (*open)(struct vsuper_block* this, struct vdir_ent* entry);
};

/** \struct vmount */
/** \brief Keep track of mounts */
struct vmount {
        /** \var mount_point */
        /** \brief To which director entry is this mounted? */
        struct vdir_ent *mount_point;
        struct vmount* next;
};

int vfs_init();
struct vfile* vfs_create();

#ifdef __cplusplus
}
#endif

#endif
