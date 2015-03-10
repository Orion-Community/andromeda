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
#include <thread.h>
#include <ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_NAME_SIZE 0x100

extern unsigned int virt_bus;
extern unsigned int lgcy_bus;
extern struct tree_root* dev_tree;

/** \typedef enum device_type_t */
typedef enum {
        VIRTUAL_BUS, /** \enum virtual_bus */
        NET_CORE_DEV, /** \enum net_core_dev */
        NET_DEV, /** \enum net_dev */
        DISK, /** \enum disk */
        PARTITION, /** \enum partition */
        TTY, /** \enum tty */
        CPU, /** \enum cpu */
        APIC_DEV, /** \enum apic */
        TIMER_DEV, /** \enum timer */
        PCI, /** \enum pci */
        USB, /** \enum usb */
        ATA, /** \enum ata aka ide */
        GRAPHICS /** \enum graphics */
} device_type_t;

struct device;

/** \struct driver */
struct driver {
        /**
         * \fn detect(dev)
         * \brief return a list of all attached devices
         * \fn attach(dev, child)
         * \brief attach a device to this device
         * \fn detach(dev, child)
         * \brief detach a device from this device
         * \fn suspend(dev)
         * \brief suspend this device and all of its attached children.
         * \fn resume(dev)
         * \brief resume this device and all of its attached children
         */
        struct device* (*detect)(struct device* dev);
        struct device* (*find)(unsigned int dev_id);
        struct device* (*find_type)(struct device* dev, device_type_t type);
        int (*attach)(struct device* dev, struct device* child);
        int (*detach)(struct device* dev, struct device* child);
        int (*suspend)(struct device* dev);
        int (*resume)(struct device* dev);
        /**
         * \var io
         *  \brief ptr to the file descriptor associated with the device.
         */
        struct vfile* io;

        mutex_t driver_lock; /** \var lock */
        /**
         * \var attach_cnt
         *  \brief how many times has the driver been attached.
         */
        atomic_t attach_cnt;
};

/** \struct device */
struct device {
        /** \fn open(this) */
        struct vfile* (*open)(struct device* this);

        /**
         * \var parent
         * \var children
         * \var next
         * \brief The pointer next in the list of children
         */
        struct device *parent;
        struct device *children;
        struct device *next;

        /**
         * \var dev_id
         * \brief The unique device identifier
         */
        int32_t dev_id;

        /** \var name */
        char name[DEVICE_NAME_SIZE];

        /** \var type */
        device_type_t type;

        /** \var driver */
        struct driver *driver;

        /** \var device_data_size */
        /** \var device_data */
        size_t device_data_size;
        void* device_data;

        /** \var lock */
        mutex_t lock;

        boolean suspended;
};
extern struct device dev_root;

/**
 * \fn get_root_device()
 * \brief Returns the andromeda root device.
 *
 * @return The andromeda root device.
 */
static inline struct device*
get_root_device()
{
        return &dev_root;
}

static inline struct vfile*
device_open_driver_io(struct device *dev)
{
        return dev->driver->io;
}

/**
 * \fn dev_init
 * \brief Kick the driver model into life.
 */
int dev_init();
int device_recurse_resume(struct device* this);
int device_recurse_suspend(struct device* this);
int device_attach(struct device* this, struct device* child);
int device_detach(struct device* this, struct device* child);
struct device* device_find_id(unsigned int dev_id);
int device_id_alloc(struct device* dev);
int dev_setup_driver(struct device *dev, fs_read_hook_t io_read,
                fs_write_hook_t io_write,
                int (*ioctl)(struct vfile* file, ioctl_t request, void* data));
struct device *dev_find_devtype(struct device *dev, device_type_t type);

#ifdef __cplusplus
}
#endif

#endif
