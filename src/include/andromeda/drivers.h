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

#ifndef __ANDROMEDA_DRIVERS_H
#define __ANDROMEDA_DRIVERS_H

#include <stdlib.h>
#include <fs/vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_SIZE 0x100

typedef enum {
        virtual_bus,
        disk,
        partition,
        tty,
        cpu,
        apic,
        pit,
        pci,
        usb,
        ata
} device_type_t;

struct device;

struct driver
{
        struct device* (*detect)(struct device* dev);
        int (*attach)(struct device* dev);
        int (*detach)(struct device* dev);
        int (*suspend)(struct device* dev);
        int (*resume)(struct device* dev);

        struct vfile *io;

        mutex_t driver_lock;
        uint32_t attach_cnt;
};

struct device
{
        struct vfile* (*open)(struct device* this);

        struct device *parent;
        struct device *children;

        struct device *next;

        char   name[DEVICE_NAME_SIZE];

        device_type_t type;

        struct driver *driver;

        size_t device_data_size;
        void*  device_data;

        mutex_t device_lock;
};

int dev_init();

#ifdef __cplusplus
}
#endif


#endif