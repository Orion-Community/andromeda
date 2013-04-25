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

#include <andromeda/drivers.h>
#include <andromeda/system.h>
#include <drivers/virt.h>
#include <fs/vfs.h>
#include <stdio.h>
#include <stdlib.h>

static struct vfile*
virt_drive_buffered_open(struct device* this)
{
        return ((struct buffer*)this->device_data)->duplicate(this->device_data);
}

struct device*
virt_drive_init(size_t size, void* data, rd_t type)
{
        struct device* drive = kmalloc(sizeof(struct device));
        if (drive == NULL)
                return NULL;

        memset(drive, 0, sizeof(struct device));

        drive->driver = kmalloc(sizeof(struct driver));
        if (drive->driver == NULL)
        {
                kfree (drive);
                return NULL;
        }

        drive->device_data = data;
        if (type == buffered)
        {
                drive->open = virt_drive_buffered_open;
        }

        return drive;
}
