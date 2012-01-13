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

/** \typedef enum device_type_t */
typedef enum {
        virtual_bus,    /** \enum virtual_bus */
        disk,           /** \enum disk */
        partition,      /** \enum partition */
        tty,            /** \enum tty */
        cpu,            /** \enum cpu */
        apic,           /** \enum apic */
        pit,            /** \enum pit */
        pci,            /** \enum pci */
        usb,            /** \enum usb */
        ata             /** \enum ata aka ide */
} device_type_t;

struct device;

/** \struct driver */
struct driver
{
        struct device* (*detect)(struct device* dev);   /** \fn detect(dev) */
        int (*attach)(struct device* dev);              /** \fn attach(dev) */
        int (*detach)(struct device* dev);              /** \fn detach(dev) */
        int (*suspend)(struct device* dev);             /** \fn suspend(dev) */
        int (*resume)(struct device* dev);              /** \fn resume(dev) */
        /** \var io
         *  \brief ptr to the file descriptor associated with the device. */
        struct vfile *io;

        mutex_t driver_lock; /** \var lock */
        /** \var attach_cnt
         *  \brief how many times has the driver been attached. */
        atomic_t attach_cnt;
};

/** \struct device */
struct device
{
        /** \fn open(this) */
        struct vfile* (*open)(struct device* this);

        /** \var parent */
        /** \var children */
        struct device *parent;
        struct device *children;
        /** \var next */
        /** \brief The pointer next in the list of children */
        struct device *next;

        /** \var name */
        char   name[DEVICE_NAME_SIZE];

        /** \var type */
        device_type_t type;

        /** \var driver */
        struct driver *driver;

        /** \var device_data_size */
        /** \var device_data */
        size_t device_data_size;
        void*  device_data;

        /** \var lock */
        mutex_t device_lock;
};

/**
 * \fn dev_init
 * \brief Kick the driver model into life.
 */
int dev_init();

#ifdef __cplusplus
}
#endif


#endif