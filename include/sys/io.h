/*
 *   Port input/output functions.
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#ifndef __IO_H_
#define __IO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * inb reads from a given port and returns the read data.
 */
extern uint8_t inb(uint16_t port);


/*
 * outb writes given data to a given port.
 */
extern void outb(uint16_t port, uint8_t data);

extern void outl(uint16_t port, uint32_t data);

extern uint32_t inl(uint16_t port);

/*
 * This function waits for data when there is written to an output port.
 */
extern void iowait();

#ifdef __cplusplus
}
#endif

#endif