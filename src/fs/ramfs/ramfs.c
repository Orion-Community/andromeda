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
#include <fs/vfs.h>
#include <fs/ramfs.h>
#include <andromeda/drivers.h>
#include <andromeda/system.h>

#warning RAMFS needs implementing

struct vsuper_block*
ram_fs_init(struct vfile* drive)
{
        struct vsuper_block* super = kmalloc(sizeof(*super));
        if (super == NULL)
                return NULL;
        memset(super, 0, sizeof(*super));

#ifdef SLAB
        super->dev = (drive == NULL) ? mm_cache_alloc(vsuper_cache, 0) : drive;
#else
        super->dev = (drive == NULL) ? kmalloc(sizeof(*drive)) : drive;
#endif

        if (super->dev == NULL)
        {
                kfree(super);
                return NULL;
        }
        return super;
}

struct vfile*
ram_fs_open(struct vsuper_block* this)
{
        if (this == NULL)
                return NULL;
        return NULL;
}

int
ram_fs_close(struct vfile* this)
{
        if (this == NULL)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

int
ram_fs_read(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

int
ram_fs_write(struct vfile* this, char* buf, size_t num)
{
        if (this == NULL || buf == NULL || num == 0)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

