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
#include <drivers/virt.h>

static struct device*
legacy_bus_detect(struct device* dev)
{
        if (dev != NULL)
                return dev->children;
        return NULL;
}

static int
legacy_bus_suspend(struct device* this)
{
        mutex_lock(&this->lock);
        if (this->suspended == TRUE)
        {
                mutex_unlock(&this->lock);
                return -E_SUCCESS;
        }

        device_recurse_suspend(this);

        this->suspended = TRUE;

        mutex_unlock(&this->lock);
        return -E_SUCCESS;
}

static int
legacy_bus_resume(struct device* this)
{
        mutex_lock(&this->lock);
        if (this->suspended != FALSE)
        {
                mutex_unlock(&this->lock);
                return -E_SUCCESS;
        }

        this->suspended = FALSE;

        device_recurse_resume(this);

        mutex_unlock(&this->lock);
        return -E_SUCCESS;
}

int
drv_legacy_bus_init(struct device* dev, struct device* parent)
{
        if (dev == NULL)
                return -E_NULL_PTR;

        memset(dev, 0, sizeof(struct device));

        dev->driver = kmalloc(sizeof(struct driver));

        if (dev->driver == NULL)
                panic("Out of memory in intialisiation of legacy bus");

        memset(dev->driver, 0, sizeof(struct driver));

        dev->driver->detect = legacy_bus_detect;
        dev->driver->attach = device_attach;
        dev->driver->detach = device_detach;
        dev->driver->suspend = legacy_bus_suspend;
        dev->driver->resume = legacy_bus_resume;

        dev->driver->find = device_find_id;
        device_id_alloc(dev);

        lgcy_bus = dev->dev_id;
        debug("Legacy bus is : %X\n", lgcy_bus);

        dev->type = legacy_bus;

        parent->driver->attach(parent, dev);

        return -E_SUCCESS;
}
