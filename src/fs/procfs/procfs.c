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
        return NULL;
}

struct vfile*
proc_fs_open(struct vsuper_block* this)
{
        return NULL;
}

int
proc_fs_close(struct vfile* this)
{
        return -E_NOFUNCTION;
}

proc_fs_seek(struct vfile* this, size_t idx, seek_t type)
{
        return -E_NOFUNCTION;
}

proc_fs_read(struct vfile* this, char* buf, size_t num)
{
        return -E_NOFUNCTION;
}

proc_fs_write(struct vfile* this, char* buf, size_t num)
{
        return -E_NOFUNCTION;
}
