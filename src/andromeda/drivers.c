/*
 *  Andromeda, The educational operatingsystem
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

#include <stdlib.h>
#include <andromeda/drivers.h>
#include <andromeda/system.h>
#include <drivers/root.h>
#include <fs/vfs.h>


static int drv_setup_io(struct device *dev, struct driver *drv,
                        struct vfile *io, vfs_read_hook_t, vfs_write_hook_t);

struct device dev_root;
uint64_t dev_id = 0;
mutex_t dev_id_lock;

uint64_t virt_bus = 0;
uint64_t lgcy_bus = 0;

/** Return the number of detected CPU's */
int dev_detect_cpus(struct device* root)
{
        return -E_NOFUNCTION;
}

int dev_root_init()
{
        struct device* root = &dev_root;
        memset(root, 0, sizeof(struct device));
        root->driver = kmalloc(sizeof(struct driver));
        memset(root->driver, 0, sizeof(struct driver));

        drv_root_init(root); /** Call to driver, not device!!! */

//         if (dev_detect_cpus(root) <= 0)
//         {
//                 panic("Couldn't find any CPU's!");
//         }

        return -E_SUCCESS;
}

int device_recurse_suspend(struct device* this)
{
        struct device* carriage = this->children;

        while(carriage != NULL)
        {
                carriage->driver->suspend(carriage);
                carriage = carriage->next;
        }
        return -E_SUCCESS;
}

int device_recurse_resume(struct device* this)
{
        struct device* carriage = this->children;

        while(carriage != NULL)
        {
                carriage->driver->resume(carriage);
                carriage = carriage->next;
        }
        return -E_SUCCESS;
}

int device_attach(struct device* this, struct device* child)
{
        if (this->children == NULL)
        {
                this->children = child;
                return -E_SUCCESS;
        }
        struct device* carriage = this->children;
        struct device* last = carriage;
        while (carriage != NULL)
        {
                last = carriage;
                carriage = carriage->next;
        }
        last->next = child;
        child->parent = this;
        return -E_SUCCESS;
}

int device_detach(struct device* this, struct device* child)
{
        struct device* carriage = this->children;
        struct device* last = carriage;
        while (carriage != NULL)
        {
                if (carriage == child)
                {
                        if (carriage == last)
                        {
                                this->children = carriage->next;
                                return -E_SUCCESS;
                        }
                        else
                        {
                                last->next = carriage->next;
                                carriage->next = NULL;
                                return -E_SUCCESS;
                        }
                }
        }
        return -E_NOTFOUND;
}

struct device*
device_find_id(struct device* this, uint64_t dev_id)
{
        if (this == NULL)
                return NULL;

        if (dev_id == this->dev_id)
                return this;

        struct device* carriage = this->children;

        struct device *dev = NULL;
        while (carriage != NULL)
        {
                if (carriage->driver == NULL)
                        panic("Driverless attached device!");
                if (carriage->driver->find == NULL)
                        panic("No find function in device!");

                dev = carriage->driver->find(carriage, dev_id);
                if (dev != NULL)
                        return dev;

                carriage = carriage->next;
        }
        return NULL;
}

int device_id_alloc(struct device* dev)
{
        int ret = 0;
        uint64_t begin = dev_id-1;
        mutex_lock(&dev_id_lock);
        struct device* iterator = device_find_id(&dev_root, dev_id);
        while (iterator != NULL)
        {
                dev_id++;
                if (dev_id == begin)
                        panic("No more room for new devices!");
                iterator = device_find_id(&dev_root, dev_id);
        }

        dev->dev_id = dev_id;
        ret = dev_id;

        mutex_unlock(&dev_id_lock);
        return ret;
}

static int
drv_setup_io(dev, drv, io, read, write)
struct device *dev;
struct driver *drv;
struct vfile *io;
vfs_read_hook_t read;
vfs_write_hook_t write;
{
        drv->io = io;
        io->uid = 0;
        io->gid = 0;
        io->read = read;
        io->write = write;
        io->type = file;

        return -E_SUCCESS;
}

int
dev_setup_driver(struct device *dev, vfs_read_hook_t read, vfs_write_hook_t write)
{
        struct driver *drv = kmalloc(sizeof(*drv));
        struct vfile *file = kmalloc(sizeof(*file));
        dev->children = NULL;
        dev->parent = NULL;
        drv_setup_io(dev,drv,file,read,write);
        dev->driver = drv;

        drv->driver_lock = 0;
        drv->attach_cnt.cnt = 0;
        drv->attach_cnt.lock = 0;
        /*
         * Setup some function pointers.
         */
        drv->attach = &device_attach;
        drv->detach = &device_detach;
        drv->resume = &device_recurse_resume;
        drv->suspend = &device_recurse_suspend;
        drv->find_type = &dev_find_devtype;
        drv->find = &device_find_id;

        return -E_SUCCESS;
}

struct device *
dev_find_devtype(struct device *dev, device_type_t type)
{
        if(dev != NULL)
        {
                if(dev->type == type)
                        return dev;
                else
                {
                        struct device *carriage;
                        for_each_ll_entry(dev->children, carriage)
                        {
                                if(carriage->type == type)
                                        return carriage;
                                if(carriage->next == NULL)
                                        return NULL;
                                else
                                        continue;
                        }
                }
        }
        return NULL;
}

void dev_dbg()
{
        int i = 0;
        for (; i < 0x10; i++)
        {
                printf("Device 0x%X at address %X of type %X\n",
                                               i, device_find_id(&dev_root, i),
                                              device_find_id(&dev_root, i)->type
                      );
        }
}

int
dev_init()
{
        debug("Building the device tree\n");

        dev_root_init();

#ifdef DEV_DBG
        dev_dbg();
#endif

        return -E_SUCCESS;
}
