/*
 *  Andromeda
 *  Copyright (C)  Bart Kuivenhoven
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
#include <andromeda/system.h>
#include <drivers/vga_text.h>

static atomic_t vga_text_count;
static struct device* vga_dev = NULL;

#warning Implement this to work with VGA text output

static size_t vga_text_read(struct vfile* this __attribute__((unused)),
                char* buf __attribute__((unused)),
                size_t start __attribute__((unused)),
                size_t idx __attribute__((unused)))
{
        /* What exactly do you intend to read from VGA? */
        return 0;
}

static size_t vga_text_write(struct vfile* this, char* buf,
                size_t idx __attribute__((unused)), size_t num)
{
        if (this == NULL || buf == NULL || num == 0 || this->out_stream == NULL) {
                return 0;
        }

        /* Do VGA writy things here */

        return 0;
}

static struct vfile* vga_text_open(struct device *this)
{
        if (this == NULL || this->driver == NULL) {
                return NULL ;
        }

        warning("vga_text_open not implemented!\n");

        this->driver->io = vfs_create();
        if (this->driver->io == NULL) {
                return NULL ;
        }
        this->driver->io->in_stream = NULL;

        return NULL ;
}
static struct device* vga_text_detect(struct device* this)
{
        return this->children;
}

int vga_text_init(struct device* parent)
{
        if (parent == NULL || parent->driver == NULL)
                return -E_NULL_PTR;

        struct device* this = kmalloc(sizeof(struct device));
        if (this == NULL)
                return -E_NOMEM;

        this->driver = kmalloc(sizeof(struct driver));
        if (this->driver == NULL) {
                kfree(this);
                return -E_NOMEM;
        }
        memset(this->driver, 0, sizeof(struct driver));

        atomic_inc(&vga_text_count);
        vga_dev = this;

        dev_setup_driver(this, vga_text_read, vga_text_write, NULL, NULL);

        this->device_data = (void*) 0xB8000;
        this->device_data_size = 0x1000;

        this->type = graphics;

        this->open = vga_text_open;
        this->driver->detect = vga_text_detect;

        parent->driver->attach(parent, this);
        device_id_alloc(this);

        this->open = vga_text_open;

        return -E_SUCCESS;
}
