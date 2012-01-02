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
#include <drivers/root.h>

/** The dummy functions for the root device */
int drv_root_dummy(struct device* root)
{
        return 0; // We found 0 of them since it's a virtual bus ...
}

int drv_root_suspend(struct device* root)
{
        struct device* cariage = root->children;
        while (cariage != NULL)
        {
                if (cariage->driver == NULL)
                        continue;
                if (cariage->driver->suspend != NULL)
                        cariage->driver->suspend(cariage);
                else
                        panic("Unable to suspend a device!");
        }
        return 0; // We found 0 of them since it's a virtual bus ...
}

int drv_root_resume(struct device* root)
{
        struct device* cariage = root->children;
        while (cariage != NULL)
        {
                if (cariage->driver == NULL)
                        continue;
                if (cariage->driver->resume != NULL)
                        cariage->driver->resume(cariage);
                else
                        panic("Unable to suspend a device!");
        }
        return 0; // We found 0 of them since it's a virtual bus ...
}
