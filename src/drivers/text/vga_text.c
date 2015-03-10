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

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CHAR_SIZE 2
#define VGA_LENGTH (VGA_WIDTH * VGA_HEIGHT)

static atomic_t vga_text_count;
static struct device* vga_dev = NULL;
static int32_t vga_text_id = 0;

struct vga_ctl_data {
        int cursor;
        int16_t colour;
};

static size_t vga_text_read(struct vfile* this __attribute__((unused)),
                char* buf __attribute__((unused)),
                size_t start __attribute__((unused)),
                size_t idx __attribute__((unused)))
{
        /* What exactly do you intend to read from VGA? */
        return 0;
}

static size_t vga_text_write(struct vfile* this, char* buf, size_t idx,
                size_t num)
{
        if (this == NULL || buf == NULL || num == 0 || this->out_stream == NULL) {
                return 0;
        }

        if (idx > VGA_LENGTH) {
                return 0;
        }

        if (idx + num > VGA_LENGTH) {
                num = VGA_LENGTH - idx;
        }

        struct device* dev = device_find_id(this->fs_data.device_id);
        if (dev == NULL || dev->device_data != NULL) {
                return 0;
        }

        struct vga_ctl_data* ctl_data = dev->device_data;
        if (dev->device_data_size != sizeof(*ctl_data)) {
                return 0;
        }

        if (ctl_data == NULL) {
                return 0;
        }

        size_t written = 0;
        uint16_t colour = ctl_data->colour << 8;

        uint16_t* vga_data = this->fs_data.fs_data_struct;

        for (; written < num; written++, idx++) {
                vga_data[idx] = colour | (uint16_t) buf[written];
        }

        return written;
}

static struct vfile* vga_text_open(struct device *this)
{
        if (this == NULL || this->driver == NULL) {
                return NULL ;
        }

        return this->driver->io;
}

static struct device* vga_text_detect(struct device* this)
{
        return this->children;
}

static int vga_set_colour(uint16_t colour, uint16_t reset_mask,
                uint16_t set_mask)
{
        if (vga_text_id == 0) {
                return -E_NOT_FOUND;
        }

        struct device* vga_text = device_find_id(vga_text_id);
        if (vga_text == NULL || vga_text->driver == NULL) {
                return -E_NOT_FOUND;
        }

        struct vga_ctl_data* data = vga_text->device_data;
        if (data == NULL) {
                data = kmalloc(sizeof(*data));
                if (data == NULL) {
                        return -E_OUT_OF_RESOURCES;
                }
                memset(data, 0, sizeof(*data));
        }

        data->colour &= reset_mask;
        data->colour |= (colour & set_mask);

        return -E_SUCCESS;
}

int vga_text_set_fg_colour(uint16_t colour)
{
        return vga_set_colour(colour, 0x00F0, 0x000F);
}

int vga_text_set_bg_colour(uint16_t colour)
{
        return vga_set_colour((colour << 4), 0x000F, 0x00F0);
}

static int vga_text_ioctl(struct vfile* this __attribute__((unused)),
                ioctl_t request __attribute__((unused)),
                void* data __attribute__((unused)))
{
        return -E_NOFUNCTION;
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

        dev_setup_driver(this, vga_text_read, vga_text_write, vga_text_ioctl);
        this->driver->io->fs_data.fs_data_struct = (void*) 0xB8000;
        this->driver->io->fs_data.fs_data_size = 0x1000;

        this->device_data = kmalloc(sizeof(struct vga_ctl_data));
        this->device_data_size = sizeof(struct vga_ctl_data);

        this->type = GRAPHICS;

        this->open = vga_text_open;
        this->driver->detect = vga_text_detect;

        parent->driver->attach(parent, this);
        device_id_alloc(this);
        vga_text_id = this->dev_id;

        this->open = vga_text_open;

        return -E_SUCCESS;
}

