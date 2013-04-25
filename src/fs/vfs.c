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

void init_vfs()
{
        return;
}

struct vfs_mount* root;

struct vfile*
vfs_open (struct vdir_ent* entry)
{
        if (entry == NULL || entry->super == NULL || entry->super->open == NULL)
                return NULL;

        return entry->super->open(entry->super, entry);
}

int vfs_close (struct vfile* stream)
{
        if (stream == NULL || stream->close == NULL)
                return -E_NULL_PTR;

        int ret = stream->close(stream);
        if (ret == -E_SUCCESS)
        {
                kfree(stream);
                return -E_SUCCESS;
        }
        else
                return ret;
}

int vfs_read (struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || stream->read == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_write (struct vfile* stream, char* buf, size_t num)
{
        if (stream == NULL || stream->write == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_seek (struct vfile* stream, size_t idx, seek_t from)
{
        if (stream == NULL || stream->seek == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_flush (struct vfile* stream)
{
        if (stream == NULL || stream->flush == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}

int vfs_mount(struct vfile* stream, struct vdir_ent* entry)
{
        if (stream == NULL || entry == NULL)
                return -E_NULL_PTR;

        return -E_NOFUNCTION;
}
