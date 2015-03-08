/*
 *  Andromeda, The educational operatingsystem
 *  Copyright (C) 2011, 2012, 2013, 2014, 2015  Bart Kuivenhoven
 *  Copyright (C) 2012  Michel Megens
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
#include <ioctl.h>
#include <andromeda/drivers.h>
#include <andromeda/system.h>
#include <drivers/root.h>
#include <fs/vfs.h>
#include <lib/tree.h>

static int drv_setup_vfile(struct vfile* file, fs_read_hook_t read,
                fs_write_hook_t write,
                int (*ioctl)(struct vfile*, ioctl_t, void*), int32_t dev_id);

struct device dev_root;
int32_t dev_id = 0;
mutex_t dev_id_lock;

unsigned int virt_bus = 0;
unsigned int lgcy_bus = 0;
struct tree_root* dev_tree = NULL;

/** Return the number of detected CPU's */
int dev_detect_cpus(struct device* root)
{
        if (root == NULL)
                return -E_INVALID_ARG;
        return -E_NOFUNCTION;
}

int dev_root_init()
{
        struct device* root = &dev_root;
        memset(root, 0, sizeof(*root));
        root->driver = kmalloc(sizeof(*root->driver));
        if (root->driver == NULL)
                panic("Unable to allocate");
        memset(root->driver, 0, sizeof(*root->driver));

        dev_tree = tree_new_avl();
        if (dev_tree == NULL) {
                panic("Out of memory!!!");
        }

        dev_tree->add(root->dev_id, root, dev_tree);
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

        while (carriage != NULL ) {
                carriage->driver->suspend(carriage);
                carriage = carriage->next;
        }
        return -E_SUCCESS;
}

int device_recurse_resume(struct device* this)
{
        struct device* carriage = this->children;

        while (carriage != NULL ) {
                carriage->driver->resume(carriage);
                carriage = carriage->next;
        }
        return -E_SUCCESS;
}

int device_attach(struct device* this, struct device* child)
{
        if (this->children == NULL) {
                this->children = child;
                return -E_SUCCESS;
        }
        struct device* carriage = this->children;
        struct device* last = carriage;
        while (carriage != NULL ) {
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
        while (carriage != NULL ) {
                if (carriage == child) {
                        if (carriage == last) {
                                this->children = carriage->next;
                                return -E_SUCCESS;
                        } else {
                                last->next = carriage->next;
                                carriage->next = NULL;
                                return -E_SUCCESS;
                        }
                }
        }
        return -E_NOTFOUND;
}

struct device*
device_find_id(unsigned int id)
{
        struct device* dev = (struct device*) dev_tree->find(id, dev_tree);
        return dev;
}

int device_id_alloc(struct device* dev)
{
        if (dev == NULL) {
                return -E_NULL_PTR;
        }
        if (dev->dev_id != 0) {
                return -E_ALREADY_INITIALISED;
        }

        int32_t idx = dev_id;

        mutex_lock(&dev_id_lock);
        int overflow = FALSE;
        while (device_find_id(idx) != NULL ) {
                idx++;
                if (idx < 0 && !overflow) {
                        overflow = TRUE;
                        idx = 1;
                }
                if (idx < 0 && overflow) {
                        mutex_unlock(&dev_id_lock);
                        return -E_OUT_OF_RESOURCES;
                }
        }
        dev_tree->add(idx, dev, dev_tree);
        dev_id = idx + 1;
        if (dev_id < 0)
                dev_id = 1;
        mutex_unlock(&dev_id_lock);

        dev->dev_id = idx;
        return idx;
}

static size_t drv_vfile_dummy_read(struct vfile* file __attribute__((unused)),
                char* buffer __attribute__((unused)),
                size_t idx __attribute__((unused)),
                size_t len __attribute((unused)))
{
        warning("Data was to be read through missing driver file\n");
        return 0;
}

static size_t drv_vfile_dummy_write(struct vfile* file __attribute__((unused)),
                char* buffer __attribute__((unused)),
                size_t idx __attribute__((unused)),
                size_t len __attribute__((unused)))
{
        warning("Data was to be written through missing driver file\n");
        return 0;
}

static int drv_vfile_dummy_ioctl(struct vfile* file __attribute__((unused)),
                ioctl_t request __attribute__((unused)),
                void* data __attribute__((unused)))
{
        warning("Ioctl was attempted though a missing driver pointer\n");
        return 0;
}

static int drv_setup_vfile(struct vfile* file, fs_read_hook_t read,
                fs_write_hook_t write,
                int (*ioctl)(struct vfile*, ioctl_t, void*), int32_t dev_id)
{
        if (read == NULL) {
                read = drv_vfile_dummy_read;
        }
        if (write == NULL) {
                write = drv_vfile_dummy_write;
        }
        if (ioctl == NULL) {
                ioctl = drv_vfile_dummy_ioctl;
        }

        file->uid = 0;
        file->gid = 0;
        file->type = CHAR_DEV;
        file->fs_data.read = read;
        file->fs_data.write = write;
        file->fs_data.device_id = dev_id;
        file->ioctl = ioctl;

        return -E_SUCCESS;
}

int dev_setup_driver(struct device *dev, fs_read_hook_t io_read,
                fs_write_hook_t io_write,
                int (*ioctl)(struct vfile* file, ioctl_t request, void* data))
{
        struct driver *drv = kmalloc(sizeof(*drv));
        if (drv == NULL) {
                return -E_NOMEM;
        }
        struct vfile *io_file = vfs_create();
        if (io_file == NULL) {
                kfree(drv);
                return -E_NOMEM;
        }
        memset(drv, 0, sizeof(*drv));
        memset(io_file, 0, sizeof(*io_file));

        if (device_id_alloc(dev) == -E_OUT_OF_RESOURCES) {
                kfree(drv);
                io_file->close(io_file);
                return -E_OUT_OF_RESOURCES;
        }

        dev->children = NULL;
        dev->parent = NULL;
        drv->io = io_file;
        drv_setup_vfile(io_file, io_read, io_write, ioctl, dev->dev_id);
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

        io_file->fs_data.fs_data_struct = dev;
        io_file->fs_data.fs_data_size = sizeof(*dev);

        return -E_SUCCESS;
}

struct device *
dev_find_devtype(struct device *dev, device_type_t type)
{
        if (dev != NULL) {
                if (dev->type == type)
                        return dev;
                else {
                        struct device *carriage;
                        for_each_ll_entry(dev->children, carriage)
                        {
                                if (carriage->type == type)
                                        return carriage;
                                if (carriage->next == NULL)
                                        return NULL ;
                                else
                                        continue;
                        }
                }
        }
        return NULL ;
}

void dev_dbg()
{
        int i = 0;
        for (; i < 0x10; i++) {
                printf("Device 0x%X at address %X of type %X\n", i,
                                device_find_id(i), device_find_id(i)->type);
        }
}

int dev_init()
{
        debug("Building the device tree\n");
        dev_root_init();

#ifdef DEV_DBG
        dev_dbg();
#endif

        return -E_SUCCESS;
}
