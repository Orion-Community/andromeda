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

#ifndef __FS_VFS_H
#define __FS_VFS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DIR_LIST_SIZE 0xFF

#define VFIO(fn, arg1, arg2, arg3) \
static size_t fn(struct vfile* arg1, char* arg2, size_t arg3); \
static size_t fn(struct vfile* arg1, char* arg2, size_t arg3)

struct vfile;

typedef size_t (*vfs_read_hook_t)(struct vfile*, char*, size_t);
typedef size_t (*vfs_write_hook_t)(struct vfile*, char*, size_t);

typedef enum { SEEK_SET, SEEK_CUR, SEEK_END } seek_t;
typedef enum {dir, block_dev, char_dev, file} file_type_t;

struct vsuper_block;

/** \struct vfile */
/** \brief What we're looking at now is the file descriptor */
struct vfile
{
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
        idx_t cursor;

        file_type_t type;

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
        size_t fs_data_size;
        void* fs_data;

        /**
         * \fn close (this)
         * \fn read(this, buf, num)
         * \fn write(this, buf, num)
         * \fn seek(this, idx, from)
         * \fn flush(this)
         */
        int (*close)(struct vfile* this);
        size_t (*read)(struct vfile* this, char* buf, size_t num);
        size_t (*write)(struct vfile* this, char* buf, size_t num);
        int (*seek)(struct vfile* this, int64_t idx, seek_t from);
        int (*flush)(struct vfile* this);
};

/** \struct vdir_ent */
/** \brief Hello directory entry. */
struct vdir_ent
{
        struct vsuper_block* super;
        struct vfile* data;
        char *name;
};

/** \struct vdir */
struct vdir
{
        struct vdir_ent*        entries[DIR_LIST_SIZE];
        struct vdir*            next;
        struct vsuper_block*    mounted;
};

/** \struct vsuper_block */
/** \brief The file system descriptor. */
struct vsuper_block
{
        struct vfile* fs_root;
        struct device* dev;

        size_t file_name_size;
        size_t fs_data_size;
        void* fs_data;

        struct vmount* mounts;

        struct vfile* (*open)(struct vsuper_block* this, struct vdir_ent* entry);
};

/** \struct vmount */
/** \brief Keep track of mounts */
struct vmount
{
        /** \var mount_point */
        /** \brief To which director entry is this mounted? */
        struct vdir_ent *mount_point;
        struct vmount* next;
};

struct vfile*   vfs_open(struct vdir_ent* entry);
int             vfs_close(struct vfile* stream);
int             vfs_read(struct vfile* stream, char* buf, size_t num);
int             vfs_write(struct vfile* stream, char* buf, size_t num);
int             vfs_seek(struct vfile* stream, size_t idx, seek_t from);

#ifdef __cplusplus
}
#endif

#endif
