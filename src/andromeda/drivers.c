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
#include <drivers/root.h>

struct device dev_root;

/** Return the number of detected CPU's */
int dev_detect_cpus(struct device* root)
{
        return -E_NOFUNCTION;
}

int dev_root_init()
{
        struct device* root = &dev_root;
        memset(root, 0, sizeof(struct device));
        root->driver = kalloc(sizeof(struct driver));
        memset(root->driver, 0, sizeof(struct driver));

        if (dev_detect_cpus(root) <= 0)
        {
                panic("Couldn't find any CPU's!");
        }

        return -E_SUCCESS;
}

int device_recurse_suspend(struct device* this)
{
        struct device* cariage = this->children;

        while(cariage != NULL)
        {
                cariage->driver->suspend(cariage);
                cariage = cariage->next;
        }
        return -E_SUCCESS;
}

int device_recurse_resume(struct device* this)
{
        struct device* cariage = this->children;

        while(cariage != NULL)
        {
                cariage->driver->resume(cariage);
                cariage = cariage->next;
        }
        return -E_SUCCESS;
}

int device_attach(struct device* this, struct device* child)
{
        struct device* cariage = this->children;
        struct device* last = cariage;
        while (cariage != NULL)
        {
                last = cariage;
                cariage = cariage->next;
        }
        last->next = child;
}

int device_detach(struct device* this, struct device* child)
{
        struct device* cariage = this->children;
        struct device* last = cariage;
        while (cariage != NULL)
        {
                if (cariage == child)
                {
                        if (cariage == last)
                        {
                                this->children = cariage->next;
                        }
                        else
                        {
                                last->next = cariage->next;
                                cariage->next = NULL;
                        }
                }
        }
}

int
dev_init()
{
        printf("Building the device tree\n");

        dev_root_init();

        return -E_SUCCESS;
}