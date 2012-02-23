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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Write a dword, word or byte towards memory space.
 */
#define __writel(addr, value) (*((volatile uint32_t*)(addr)) = (value));
#define __writew(addr, value) (*((volatile uint16_t*)(addr)) = (value));
#define __writeb(addr, value) (*((volatile uint8_t*)(addr)) = (value));

/*
 * Read a dword, word or byte from memory space
 */
#define __readl(addr) (*((volatile uint32_t*)(addr)))
#define __readw(addr) (*((volatile uint16_t*)(addr)))
#define __readb(addr) (*((volatile uint8_t*)(addr)))

/*
 * generic i/o registers
 */
#define CMOS_SELECT 0x70
#define CMOS_DATA   0x71

static inline void
writel(volatile void* addr, uint32_t data)
{
  __writel(addr, data);
}

static inline void
writew(volatile void* addr, uint16_t data)
{
  __writew(addr, data);
}

static inline void
writeb(volatile void* addr, uint8_t data)
{
  __writeb(addr, data);
}

static inline uint32_t
readl(volatile void* addr)
{
  return __readl(addr);
}

static inline uint16_t
readw(volatile void* addr)
{
  return __readw(addr);
}

static inline uint8_t
readb(volatile void* addr)
{
  return __readb(addr);
}

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

#ifdef __cplusplus
}
#endif

#endif

/** \file */
