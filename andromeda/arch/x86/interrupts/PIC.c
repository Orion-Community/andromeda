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
#ifndef __COMPRESSED
#include <interrupts.h>
#include <error/panic.h>
#include <PIC/PIC.h>
#include <io.h>

void picRemap(int offset1, int offset2)
{
  unsigned char a1, a2;
  a1 = inb(PIC1DATA); // Save masks
  a2 = inb(PIC2DATA);
  
  outb(PIC1COMMAND, ICW1_INIT | ICW1_ICW4); // Tell the master that we will initialise with ICW4
  outb(PIC2COMMAND, ICW1_INIT | ICW1_ICW4); // Tell the slave that we will initialise with ICW4
  ioWait();
  outb(PIC1DATA, offset1); // Map the first set of interrupts
  outb(PIC2DATA, offset2); // Map the second set of interrupts
  ioWait();
  outb(PIC1DATA, 4); // Set the cascading accordingly
  outb(PIC2DATA, 2);
  ioWait();
  outb(PIC1DATA, ICW4_8086); // Make the PIC 8086 compatible
  outb(PIC2DATA, ICW4_8086);
  ioWait();
  outb(PIC1DATA, a1); // Restore the saved masks
  outb(PIC2DATA, a2);
}

void initPIC()
{
  pic = PIC; // Tell the rest of the world that the legacy PIC has been chosen
  picRemap(INTBASE, INTBASE+8);
  //panic("No implementation for the PIC!");
}
#endif