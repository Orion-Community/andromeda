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

IRQ(timer_irq, irq, stack)
{
        struct irq_data *data = get_irq_data(irq);
        struct device *dev = (struct device *)(data->irq_data);
        
        TIMER *timer = dev->device_data;
        timer->tick++;
        timer->tick_handle(timer);
}

VFIO(timer_write, file, data, size)
{
        TIMER *timer = (TIMER*)data;
        unsigned char cnf = 1;
        timer->config = timer->config & 0x7;
        while(timer->config)
        {
                cnf &= timer->config;
                switch(cnf)
                {
                        case 0x4:
                                timer->set_mode(timer);
                                break;
                        case 0x2:
                                timer->set_frq(timer);
                                break;
                        case 0x1:
                                /* get the tick offset */
                                if(size <= sizeof(void*))
                                        return 0;
                                uint64_t offset = *((uint64_t*)(data+
                                                                sizeof(void*)));
                                timer->set_tick(timer, offset);
                                break;

                        default: /* unknown config word.. reset bits set? */
                                break;
                }
                timer->config &= (~cnf); /* disable just checked bit */
                cnf <<= 1;
                if(cnf >= 1>>3)
                        break;
        }
        return E_SUCCESS;
}

VFIO(timer_read, file, data, size)
{
        return (size_t)((TIMER*)data)->tick;
}

TIMER *
init_timer_obj(char *name, timer_tick_t tick_handle, void *data)
{
        TIMER *timer = kzalloc(sizeof(*timer));
        if(NULL == timer)
                return timer;
        timer->name = name;
        timer->tick_handle = tick_handle;
        timer->timer_data = data;
        timer->config = 0xff;
        
        struct device *root = get_root_device();
        struct device *dev = kalloc(sizeof(*dev));
        dev_timer_init(dev, root);
        dev->device_data = timer;
        dev_timer_setup_io(dev, &timer_read, &timer_write);
        timer->id = dev->dev_id;
        
        return timer;
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

        dev->driver->attach = &device_attach;
        dev->driver->detach = &device_detach;
        dev->driver->suspend = &device_recurse_suspend;
        dev->driver->resume = &device_recurse_resume;

        dev->driver->find = &device_find_id;
        device_id_alloc(dev);

        dev->type = timer_dev;

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
        struct vfile *io = dev->driver->io;
        
        io->type = file;
        io->read = read;
        io->write = write;
        
        TIMER *timer = (TIMER*)dev->device_data;
        struct irq_data *data = alloc_irq();
        data->base_handle = &do_irq;
        data->handle = &timer_irq;
        data->irq_data = (void*)dev;
        if(!native_setup_irq_handler(data->irq))
                install_irq_vector(data);
        else
                return -E_GENERIC;
}
