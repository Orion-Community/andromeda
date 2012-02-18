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
#include <andromeda/irq.h>
#include <andromeda/timer.h>

#include <fs/vfs.h>

#include <drivers/root.h>

IRQ(timer, irq, stack)
{
        struct device *dev;
        struct idata data = get_irq_data(irq);
        dev = data->irq_data;
        
        TIMER *timer = dev->device_data;
        timer->timer_tick(timer);
}

static TIMER *
init_timer_obj(char *name, timer_tick_t tick_handle, void *data)
{
        TIMER *timer = kzalloc(sizeof(*timer));
        if(NULL == timer)
                return timer;
        timer->name = name;
        timer->tick_handle = tick_handle;
        timer->data = data;
        
        struct device *root = get_root_device();
        struct device *dev = kalloc(sizeof(*dev));
        dev_timer_init(dev, root);
        dev_timer_setup_io(dev);
}

static int 
dev_timer_init(struct device* dev, struct device* parent)
{
        if (dev == NULL || parent == NULL)
                return -E_NULL_PTR;

        memset(dev, 0, sizeof(struct device));

        dev->driver = kalloc(sizeof(struct driver));

        if (dev->driver == NULL)
                panic("Out of memory in intialisiation of timer device\n");

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

static int
dev_timer_setup_io(dev, read, write)
struct device *dev;
vfs_read_hook_t read;
vfs_write_hook_t write;
{
        dev->driver->io = kzalloc(sizeof(*(dev->driver->io)));
}
