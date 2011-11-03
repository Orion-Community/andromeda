/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#ifndef __IO_H_
#define __IO_H_

/*
 * Write a dword, word or byte towards memory space.
 */
#define writel(addr, value) (*((volatile uint32_t*)(addr)) = (value));
#define writew(addr, value) (*((volatile uint16_t*)(addr)) = (value));
#define writeb(addr, value) (*((volatile uint8_t*)(addr)) = (value));

/*
 * Read a dword, word or byte from memory space
 */
#define readl(addr) (*((volatile uint32_t*)(addr)))
#define readw(addr) (*((volatile uint16_t*)(addr)))
#define readb(addr) (*((volatile uint8_t*)(addr)))

void
outb(uint16_t, uint8_t);

void
outw(uint16_t, uint16_t);

void
outl(uint16_t, uint32_t);

uint8_t
inb(uint16_t);

uint16_t
inw(uint16_t);

uint32_t
inl(uint16_t);

void
iowait(void);
#define ioWait(void) iowait(void);


extern unsigned int
readCR0(void);

extern unsigned int
readCR3(void);

extern void
writeCR0(unsigned int);

extern void
writeCR3(unsigned int);

#endif
