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

#ifndef __DRIVERS_VIRT_H
#define __DRIVERS_VIRT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
        buffered,
        single_block
} rd_t;

int drv_virt_bus_init(struct device* dev, struct device* parent);
struct device* virt_drive_init(size_t size, void* data, rd_t type);

#ifdef __cplusplus
}
#endif

#endif
