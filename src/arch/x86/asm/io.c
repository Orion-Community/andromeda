/*
 *   The openLoader project - Inline I/O functions.
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

#include <io.h>
#include <stdlib.h>

void
outb (uint16_t port, uint8_t data)
{
  __asm__ __volatile__("outb %%al, %%dx"
                       : /* no output */
                       : "a" (data),
                       "d" (port)
                       );
}

void
outw (uint16_t port, uint16_t data)
{
  __asm__ __volatile__("outw %%ax, %%dx"
                       : /* no output */
                       : "a" (data),
                       "d" (port)
                       );
}

void
outl (uint16_t port, uint32_t data)
{
  __asm__ __volatile__("outl %%eax, %%dx"
                       : /* no output */
                       : "a" (data),
                       "d" (port)
                       );
}

uint8_t
inb (uint16_t port)
{
  register uint8_t ret;
  __asm__ __volatile__("inb %%dx, %%al"
                       : "=a" (ret)
                       : "d" (port)
                       );
  return ret;
}

uint16_t
inw (uint16_t port)
{
  register uint16_t ret;
  __asm__ __volatile__("inw %%dx, %%ax"
                       : "=a" (ret)
                       : "d" (port)
                       );
  return ret;
}

uint32_t
inl (uint16_t port)
{
  register uint32_t ret;
  __asm__ __volatile__("inl %%dx, %%eax"
                       : "=a" (ret)
                       : "d" (port)
                       );
  return ret;
}

void
iowait (void)
{
  __asm__ __volatile__("xorl %%eax, %%eax\n\t"
                       "outb %%al, $0x80"
                       : /* no output */
                       : /* no input */
                       : "%eax");
}

