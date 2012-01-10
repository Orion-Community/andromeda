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
        warning("vga_text_resume not implemented!");
        return -E_NOFUNCTION;
}

static int vga_text_detach(struct device* this)
{
        warning("vga_text_detach not implemented!");
        return -E_NOFUNCTION;
}

static int vga_text_suspend(struct device* this)
{
        warning("vga_text_suspend not implemented!");
        return -E_NOFUNCTION;
}

/**
 * return the first, unattached vga_text device
 */
struct device* vga_text_detect(struct device* this)
{
        mutex_lock(vga_text_lock);
        if (vga_text_count != 0)
        {
                mutex_unlock(vga_text_lock);
                return NULL;
        }

        struct device* dev = kalloc(sizeof(struct device));
        if (dev == NULL)
        {
                mutex_unlock(vga_text_lock);
                return NULL;
        }
        memset(dev, 0, sizeof(struct device));

        dev->device_data = (void*)0xB8000;
        dev->device_data_size = 0x1000;

        if (vga_text_attach(dev) != -E_SUCCESS)
        {
                free(dev);
                mutex_unlock(vga_text_lock);
                return NULL;
        }
        vga_dev = dev;

        vga_text_count++;
        mutex_unlock(vga_text_lock);
        return dev;
}

/**
 * Attach the basic driver functions to the driver structure
 */
int vga_text_attach(struct device* this)
{
        if (this->driver == NULL)
                this->driver = kalloc(sizeof(struct driver));
        if (this->driver == NULL)
                return -E_NOMEM;

        this->open = vga_text_open;
        this->driver->attach = vga_text_attach;
        this->driver->detect = vga_text_detect;
        this->driver->suspend = vga_text_suspend;
        this->driver->detach = vga_text_detach;
        this->driver->resume = vga_text_resume;

        return -E_SUCCESS;
}
