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
#include <andromeda/idt.h>
#include <andromeda/system.h>
#include <arch/x86/idt.h>

#include <fs/vfs.h>

#include <drivers/root.h>

static int dev_timer_init(struct device* dev, struct device* parent);
static int dev_timer_setup_io(struct device*, vfs_read_hook_t, vfs_write_hook_t);
static int setup_timer_irq(TIMER *timer, bool forse_vec, unsigned char vector);

IRQ(timer_irq, irq, stack)
{
        struct irq_data *data = get_irq_data(irq);
        struct device *dev = (struct device *)(data->irq_data);

        TIMER *timer = dev->device_data;
        timer->tick++;
        timer->tick_handle(timer);

        return;
}

VFIO(timer_write, file, data, size)
{
        TIMER *timer = (TIMER*)data;
        unsigned char cnf = 1, i = 1;
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
                cnf = (++i)<<1;
                if(cnf >= 1>>3)
                        break;
        }
        return E_SUCCESS;
}

VFIO(timer_read, file, data, size)
{
        return (size_t)((TIMER*)data)->tick;
}

static TIMER *
create_timer_obj(char *name, timer_tick_t tick_handle, void *data)
{
        TIMER *timer = kmalloc(sizeof(*timer));
        if(NULL == timer)
                return timer;
        memset(timer, 0, sizeof(*timer));
        timer->name = name;
        timer->tick_handle = tick_handle;
        timer->timer_data = data;
        timer->config = 0xff;

        struct device *root = get_root_device();
        struct device *dev = kmalloc(sizeof(*dev));
        dev_timer_init(dev, root);
        dev->device_data = timer;
        timer->id = dev->dev_id;
        dev_timer_setup_io(dev, &timer_read, &timer_write);

        return timer;
}

TIMER *
init_timer_obj(char *name, timer_tick_t tick_handle, void *data, bool forse_vec,
                                                        unsigned char vec)
{
        TIMER *timer = create_timer_obj(name, tick_handle, data);
        if(NULL == timer)
                return timer;
        setup_timer_irq(timer, forse_vec, vec);
        return timer;
}

static int
dev_timer_init(struct device* dev, struct device* parent)
{
        if (dev == NULL || parent == NULL)
                return -E_NULL_PTR;

        memset(dev, 0, sizeof(struct device));

        dev->driver = kmalloc(sizeof(struct driver));

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
        dev->driver->io = kmalloc(sizeof(*(dev->driver->io)));
        memset(dev->driver->io, 0, sizeof(*(dev->driver->io)));
        struct vfile *io = dev->driver->io;

        io->type = file;
        io->read = read;
        io->write = write;
        return -E_SUCCESS;
}

static int
setup_timer_irq(TIMER *timer, bool forse_vec, unsigned char vector)
{
        struct irq_data *data = alloc_irq();
        struct device *root = get_root_device();
        data->base_handle = &do_irq;
        data->handle = &timer_irq;
        data->irq_data = root->driver->find(root, timer->id);

        if(forse_vec)
        {
                data->irq_config->vector = vector;
        }
        if(!native_setup_irq_handler(data->irq))
        {
                install_irq_vector(data);
                return -E_SUCCESS;
        }
        else
                return -E_GENERIC;
}
