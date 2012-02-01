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

#include <andromeda/drivers.h>
#include <drivers/root.h>

int drv_timer_init(struct device* dev, struct device* parent)
{
        if (dev == NULL || parent == NULL)
                return -E_NULL_PTR;

        memset(dev, 0, sizeof(struct device));

        dev->driver = kalloc(sizeof(struct driver));

        if (dev->driver == NULL)
                panic("Out of memory in intialisiation of timer device");

        memset(dev->driver, 0, sizeof(struct driver));

        dev->driver->detect = device_detect;
        dev->driver->attach = device_attach;
        dev->driver->detach = device_detach;
        dev->driver->suspend = device_suspend;
        dev->driver->resume = device_resume;

        dev->driver->find = device_find_id;
        device_id_alloc(dev);

        dev->type = legacy_bus;

        parent->driver->attach(parent, dev);

        return -E_SUCCESS;
}