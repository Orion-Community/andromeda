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

struct vsuper_block*
proc_fs_init(struct device* drive)
{
        if (drive == NULL)
                return NULL;
        return NULL;
}

struct vfile*
proc_fs_open(struct vsuper_block* this)
{
        if (this == NULL)
                return NULL;
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
proc_fs_seek(struct vfile* this, size_t idx, seek_t type)
{
        if (this == NULL)
                return -E_INVALID_ARG;

        panic("Using an incomplete function!");
        switch(type)
        {
        case SEEK_CUR:
        case SEEK_END:
                break;
        case SEEK_SET:
                this->cursor = idx;
                break;
        default:
                break;
        }

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
