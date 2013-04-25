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

struct ramfs_block*
ram_fs_get_block(uint64_t idx)
{
        return NULL;
}

struct vsuper_block*
ram_fs_init(struct device* drive)
{
        struct vsuper_block* super = kmalloc(sizeof(struct vsuper_block));
        if (super == NULL)
                return NULL;

        memset (super, 0, sizeof(struct vsuper_block));

        if (drive == NULL)
        {
                /**
                * Build the ram file system here
                */
                super->dev = kmalloc(sizeof(struct device));
                if (super->dev == NULL)
                        goto dev_alloc;
                memset(super->dev, 0, sizeof(struct device));
        }
        else
        {
                goto dev_alloc;
                // Going to err will do for now ...
        }

        return super;

dev_alloc:
        kfree(super);
        return NULL;
}

struct vfile*
ram_fs_open(struct vsuper_block* this)
{
        return NULL;
}

int
ram_fs_close(struct vfile* this)
{
        return -E_NOFUNCTION;
}

int
ram_fs_seek(struct vfile* this, size_t idx, seek_t type)
{
        return -E_NOFUNCTION;
}

int
ram_fs_read(struct vfile* this, char* buf, size_t num)
{
        return -E_NOFUNCTION;
}

int
ram_fs_write(struct vfile* this, char* buf, size_t num)
{
        return -E_NOFUNCTION;
}

