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

#ifndef __ANDROMEDA_DRIVERS_H
#define __ANDROMEDA_DRIVERS_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 
 */

struct device_driver
{
        uint32_t irq;
        uint32_t device_id;
//         file_t*  device_stream;
//         int (*open) (struct device_driver* driver, int device);
//         int (*close)(struct device_driver* driver, file_t* device);
//         int (*write)(struct device_driver* driver, file_t* device, char* fmt);
//         int (*read) (struct device_driver* driver, file_t* device, char* data);
//         int (*seek) (struct device_driver* driver, long long offset,
//                                                             enum seektype from);
//         int (*irq_handle)(struct device_driver* driver);
};

#ifdef __cplusplus
}
#endif


#endif