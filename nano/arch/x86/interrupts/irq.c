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

#include <arch/x86/interrupts.h>
#include <arch/x86/irq.h>
#include <arch/x86/PIC/PIC.h>
#include <arch/x86/APIC/APIC.h>
#include <stdlib.h>

void timerTick(); // The timer function
void picEOI(int irqNo); // Reset the interrupt pin
void keyboard();

void irqHandle(isrVal_t regs)
{
  switch (regs.funcPtr)
  {
    case 0x0:
      timerTick();
      break;
    case 0x1:
      keyboard();
      break;
    case 0x2:
      break;
    case 0x3:
      break;
    case 0x4:
      break;
    case 0x5:
      break;
    case 0x6:
      break;
    case 0x7:
      break;
    case 0x8:
      break;
    case 0x9:
      break;
    case 0xA:
      break;
    case 0xB:
      break;
    case 0xC:
      break;
    case 0xD:
      break;
    case 0xE:
      break;
    case 0xF:
      break;
    default:
      break;
  }
  if (pic == PIC)
  {
    picEOI(regs.funcPtr); // Send end of interrupt signal.
  }
}
unsigned long long timer = 0;
void timerTick()
{
  timer+=1;
}

void keyboard()
{
  char c = inb(0x60);
  switch(c)
  {
    case 0x1e:
      putc('a');
      break;
    case 0x30:
      putc('b');
      break;
    case 0x2e:
      putc('c');
      break;
    case 0x20:
      putc('d');
      break;
    case 0x12:
      putc('e');
      break;
    case 0x21:
      putc('f');
      break;
    case 0x22:
      putc('g');
      break;
    case 0x23:
      putc('h');
      break;
    case 0x17:
      putc('i');
      break;
    case 0x24:
      putc('j');
      break;
    case 0x25:
      putc('k');
      break;
    case 0x26:
      putc('l');
      break;
    case 0x32:
      putc('m');
      break;
    case 0x31:
      putc('n');
      break;
    case 0x18:
      putc('o');
      break;
    case 0x19:
      putc('p');
      break;
    case 0x10:
      putc('q');
      break;
    case 0x13:
      putc('r');
      break;
    case 0x1f:
      putc('s');
      break;
    case 0x14:
      putc('t');
      break;
    case 0x16:
      putc('u');
      break;
    case 0x2f:
      putc('v');
      break;
    case 0x11:
      putc('w');
      break;
    case 0x2d:
      putc('x');
      break;
    case 0x15:
      putc('y');
      break;
    case 0x2c:
      putc('z');
      break;
  }
}

void picEOI(int irqNo) // Send end of interrupt
{
  if (irqNo >= 0x7)
  {
    outb(PIC2COMMAND, PICEOI);
  }
  outb(PIC1COMMAND, PICEOI);
}