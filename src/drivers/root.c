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
#include <andromeda/system.h>
#include <drivers/root.h>
#include <drivers/virt.h>
#include <drivers/vga_text.h>
#include <drivers/legacy.h>
#include <drivers/serial.h>

static int drv_root_suspend(struct device* root)
{
        struct device* carriage = root->children;
        while (carriage != NULL ) {
                if (carriage->driver == NULL)
                        continue;
                if (carriage->driver->suspend != NULL)
                        carriage->driver->suspend(carriage);
                else
                        panic("Unable to suspend a device!");
        }
        return 0; // We found 0 of them since it's a virtual bus ...
}

static int drv_root_resume(struct device* root)
{
        struct device* carriage = root->children;
        while (carriage != NULL ) {
                if (carriage->driver == NULL)
                        continue;
                if (carriage->driver->resume != NULL)
                        carriage->driver->resume(carriage);
                else
                        panic("Unable to suspend a device!");
        }
        return 0; // We found 0 of them since it's a virtual bus ...
}

static struct device*
drv_root_detect(struct device* this)
{
        if (this == NULL)
                return NULL ;
        return this->children;
}

int init_buses(struct device* root)
{
        if (root == NULL)
                return -E_NULL_PTR;

        struct device* virtual = kmalloc(sizeof(struct device));
        if (virtual == NULL){
                return -E_NOMEM;
        }
        struct device* legacy = kmalloc(sizeof(struct device));
        if (legacy == NULL) {
                kfree(virtual);
                return -E_NOMEM;
        }

        int ret = drv_virt_bus_init(virtual, root);
        if (ret != -E_SUCCESS) {
                printf("Returned value from virtual bus: %X\n", -ret);
                panic("Could not initialise the virtual bus!");
        }
        ret = drv_legacy_bus_init(legacy, root);
        if (ret != -E_SUCCESS) {
                printf("Returned value from legacy bus: %X\n", -ret);
                panic("Could not initialise the legacy bus!");
        }

        ret = vga_text_init(device_find_id(lgcy_bus));
        if (ret != -E_SUCCESS) {
                printf("Returned value from vga: %X\n", -ret);
                panic("The text driver can't be initiated!");
        }

        ret = drv_serial_kickstart(device_find_id(lgcy_bus));
        if (ret != -E_SUCCESS) {
                printf("Returned value from serial init: %X\n", -ret);
                panic("The serial driver was not properly initialised!");
        }

        return -E_SUCCESS;

}

int drv_root_init(struct device* dev)
{
        if (dev == NULL || dev->driver == NULL)
                return -E_NULL_PTR;
        dev->type = VIRTUAL_BUS;
        dev->parent = dev;

        dev->driver->detect = drv_root_detect;
        dev->driver->suspend = drv_root_suspend;
        dev->driver->resume = drv_root_resume;

        dev->driver->attach = device_attach;
        dev->driver->detach = device_detach;

        dev->driver->find = device_find_id;
        dev->dev_id = 0;

        dev->type = VIRTUAL_BUS;

        dev->driver->io = vfs_create();
        if (dev->driver->io == NULL) {
                panic("IO file went wrong!");
        }

        if (init_buses(dev) != -E_SUCCESS) {
                panic("Could not initialise device drivers!");
        }

        return -E_SUCCESS;
}
