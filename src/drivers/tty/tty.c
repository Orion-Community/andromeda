/*
 Andromeda
 Copyright (C) 2015  Bart Kuivenhoven

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <andromeda/drivers.h>
#include <ioctl.h>
#include <stdio.h>
#include <drivers/tty.h>
#include <andromeda/system.h>
#include <lib/tree.h>

struct tty_stream {
        struct vfile* stream;
        struct tty_stream* next;
};

struct tty_line {
        int16_t length;
        struct tty_line* next;
        struct tty_line* prev;
        size_t offset;
        char line[];
};

struct tty_data {
        uint16_t line_width;
        size_t lines;

        size_t line_idx;
        mutex_t tty_lock;

        struct tree_root* input_buffer;
        struct tree_root* output_buffer;

        struct tty_stream* input_stream;
        struct tty_stream* output_stream;

        struct tty_stream* stdio_stream;
};

static size_t drv_tty_read(struct vfile* this, char* buf, size_t idx,
                size_t len)
{
        if (len == 0 || this == NULL || buf == NULL) {
                return 0;
        }

        return 0;
}

static size_t drv_tty_write(struct vfile* this, char* buf, size_t idx,
                size_t len)
{
        if (len == 0 || this == NULL || buf == NULL) {
                return 0;
        }

        return 0;
}

static int dev_tty_ioctl(struct vfile* this, ioctl_t request, void* data)
{
        if (this == NULL || data == NULL) {
                return -E_NULL_PTR;
        }

        struct device* dev = device_find_id(this->fs_data.device_id);
        if (dev == NULL) {
                return -E_INVALID_ARG;
        }

        switch (request) {
        case IOCTL_TTY_RESIZE:
                break;
        case IOCTL_TTY_CONNECT_INPUT:
                break;
        case IOCTL_TTY_DISCONNECT_INPUT:
                break;
        case IOCTL_TTY_CONNECT_OUTPUT:
                break;
        case IOCTL_TTY_DISCONNECT_OUTPUT:
                break;
        case IOCTL_TTY_GET_INPUT:
                break;
        case IOCTL_TTY_GET_OUTPUT:
                break;
        case IOCTL_TTY_GET_SIZE:
                break;
        default:
                return -E_INVALID_ARG;
                break;
        }

        return -E_SUCCESS;
}

struct device* drv_tty_start(struct device* parent, int num)
{
        if (parent == NULL) {
                return NULL ;
        }

        struct device* tty = kmalloc(sizeof(*tty));
        if (tty == NULL) {
                return NULL ;
        }

        struct tty_data* tty_data = kmalloc(sizeof(*tty_data));
        if (tty_data == NULL) {
                kfree_s(tty, sizeof(*tty));
                return NULL;
        }

        memset(tty, 0, sizeof(*tty));
        memset(tty_data, 0, sizeof(*tty_data));

        sprintf(tty->name, "tty%i", num);
        tty->type = TTY;

        dev_setup_driver(tty, drv_tty_read, drv_tty_write, dev_tty_ioctl);

        return NULL ;
}
