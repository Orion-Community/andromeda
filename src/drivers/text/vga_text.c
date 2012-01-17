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
#include <andromeda/drivers.h>
#include <drivers/vga_text.h>

static int vga_text_count = 0;
static mutex_t vga_text_lock = mutex_unlocked;
static struct device* vga_dev = NULL;

static int vga_text_read(struct vfile*  this, char* buf, size_t num)
{
        warning("vga_text_read not implemented!\n");
        return -E_NOFUNCTION;
}

static int vga_text_seek(struct vfile*  this, size_t num, seek_t from)
{
        warning("vga_text_seek not implemented!\n");
        return -E_NOFUNCTION;
}

static int vga_text_flush(struct vfile* this)
{
        warning("vga_text_flush not implemented!\n");
        return -E_NOFUNCTION;
}

static int vga_text_close(struct vfile* this)
{
        warning("ga_text_close not implemented!\n");
        return -E_NOFUNCTION;
}

static int vga_text_write(struct vfile* this, char* buf, size_t num)
{
        warning("vga_text_write not implemented!");
        return -E_NOFUNCTION;
}

static struct vfile* vga_text_open(struct device *this)
{
        warning("vga_text_open not implemented!\n");
        return NULL;
}

static int vga_text_resume(struct device* this)
{
        mutex_lock(this->lock);
        if (this->suspended != FALSE)
        {
                mutex_unlock(this->lock);
                return -E_SUCCESS;
        }

        this->suspended = FALSE;

        device_recurse_resume(this);

        mutex_unlock(this->lock);
        return -E_SUCCESS;
}

static int vga_text_suspend(struct device* this)
{
        mutex_lock(this->lock);
        if (this->suspended == TRUE)
        {
                mutex_unlock(this->lock);
                return -E_SUCCESS;
        }

        device_recurse_suspend(this);

        this->suspended = TRUE;

        mutex_unlock(this->lock);
        return -E_SUCCESS;
}

struct device* vga_text_detect(struct device* this)
{
        return this->children;
}

int vga_text_init(struct device* parent)
{
        if (parent == NULL || parent->driver)
                return -E_NULL_PTR;

        struct device* this = kalloc(sizeof(struct device));
        if (this == NULL)
                return -E_NOMEM;

        this->device_data = (void*)0xB8000;
        this->device_data_size = 0x1000;

        this->open = vga_text_open;
        this->driver->attach = device_attach;
        this->driver->detach = device_detach;
        this->driver->detect = vga_text_detect;
        this->driver->suspend = vga_text_suspend;
        this->driver->resume = vga_text_resume;

        parent->driver->attach(parent, this);

        return -E_SUCCESS;
}
