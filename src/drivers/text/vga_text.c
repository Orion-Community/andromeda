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

int vga_text_detect(struct device* this)
{
        /** Return the number of sub-devices attached to this device */
        warning("vga_text_detected is dummy!\n");
        return 0;
}

int vga_text_attach(struct device* this)
{
        if (this->driver == NULL)
                this->driver = kalloc(sizeof(struct driver));

        this->open = vga_text_open;
        this->driver->attach = vga_text_attach;
        this->driver->detect = vga_text_detect;
        this->driver->suspend = vga_text_suspend;
        this->driver->detach = vga_text_detach;
        this->driver->resume = vga_text_resume;
        return -E_SUCCESS;
}

int vga_text_detach(struct device* this)
{
        warning("vga_text_detach not implemented!");
        return -E_NOFUNCTION;
}
int vga_text_suspend(struct device* this)
{
        warning("vga_text_suspend not implemented!");
        return -E_NOFUNCTION;
}
int vga_text_resume(struct device* this)
{
        warning("vga_text_resume not implemented!");
        return -E_NOFUNCTION;
}

struct vfile* vga_text_open(struct device *this)
{
        warning("vga_text_open not implemented!\n");
        return NULL;
}

int vga_text_write(struct vfile* this, char* buf, size_t num)
{
        warning("vga_text_write not implemented!");
        return -E_NOFUNCTION;
}

int vga_text_read(struct vfile*  this, char* buf, size_t num)
{
        warning("vga_text_read not implemented!\n");
        return -E_NOFUNCTION;
}

int vga_text_seek(struct vfile*  this, size_t num, seek_t from)
{
        warning("vga_text_seek not implemented!\n");
        return -E_NOFUNCTION;
}

int vga_text_flush(struct vfile* this)
{
        warning("vga_text_flush not implemented!\n");
        return -E_NOFUNCTION;
}

int vga_text_close(struct vfile* this)
{
        warning("ga_text_close not implemented!\n");
        return -E_NOFUNCTION;
}
