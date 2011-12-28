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

#include <andromeda/drivers.h>
#include <stdlib.h>

struct device dev_root;

int dev_root_attach(struct device* dev)
{
        return -E_SUCCESS;
}

int dev_root_init()
{
        memset(&dev_root, 0, sizeof(struct device));
        (&dev_root)->driver = kalloc(sizeof(struct driver));
        memset((&dev_root)->driver, 0, sizeof(struct driver));

        (&dev_root)->driver->attach = dev_root_attach;
        return -E_SUCCESS;
}

int
dev_init()
{
        printf("Populating /dev\n");
        dev_root_init();

        (&dev_root)->driver->attach (NULL);
        return -E_SUCCESS;
}