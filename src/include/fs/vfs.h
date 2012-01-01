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

typedef enum { SEEK_SET, SEEK_CUR, SEEK_END } seek_t;

struct vfile
{
        struct vinode* data;

        uint32_t uid;
        uint32_t gid;
        uint16_t rights;
};

struct vinode
{
        struct vdir_ent *dir_ent;
        struct vsuper_block *super;
        size_t fs_data_size;
        void* fs_data;

        int (*close)(struct vinode* this);
        int (*read)(struct vinode* this, char* buf, size_t num);
        int (*write)(struct vinode* this, char* buf, size_t num);
        int (*seek)(struct vinode* this, size_t idx, seek_t from);
        int (*flush)(struct vinode* this);
};

struct vdir_ent
{
        struct vsuper_block* super;
        struct vfile *data;
        char *name;
};

struct vsuper_block
{
        struct vinode *fs_root;
        struct device* dev;

        size_t file_name_size;
        size_t fs_data_size;
        void* fs_data;

        struct vmount* mounts;

        struct vfile* (*open)(struct vsuper_block* this, struct vdir_ent* entry);
};

struct vmount
{
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
