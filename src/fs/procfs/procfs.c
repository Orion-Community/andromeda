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

#warning PROCFS still requires an implementation

#include <fs/vfs.h>
#include <andromeda/system.h>
struct vsuper_block*
proc_fs_init(struct device* drive)
{
        if (drive == NULL)
                return NULL;
        struct vsuper_block* block = kmalloc(sizeof(*block));
        if (block == NULL)
                return NULL;
        memset (block, 0, sizeof(*block));

        return block;
}

struct vfile*
proc_fs_open(struct vsuper_block* this)
{
        if (this == NULL)
                return NULL;

        struct vfile* f = kmalloc(sizeof(*f));
        if (f == NULL)
                return NULL;
        memset(f, 0, sizeof(*f));

        return NULL;
}

int
proc_fs_close(struct vfile* this)
{
        if (this == NULL)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

int
proc_fs_read(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

int
proc_fs_write(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}
