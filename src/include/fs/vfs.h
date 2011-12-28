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
        struct super_block *super;

        int (*read)();
        int (*write)();
        int (*flush)();
};

struct vdir_ent
{
        struct super_block* super;
        struct vfile *data;
        char name[];
};

struct super_block
{
        struct vinode *fs_root;

        uint32_t file_name_size;

        int (*open)();
        int (*close)();
};

struct vmount
{
        struct vdir_ent *mount_point;
};

#ifdef __cplusplus
}
#endif

#endif
