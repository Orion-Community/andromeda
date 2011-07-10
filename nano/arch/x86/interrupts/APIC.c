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
#include <interrupts/int.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/PIC/PIC.h>
#include <arch/x86/APIC/APIC.h>

#define __COMPRESSED

void intInit()
{
  prepareIDT();
  if (DetectAPIC())
  {
    printf("WARNING: The APIC hasn't got any implementation!\nFalling back to PIC support\n");
  }
  initPIC();
  sti();
}
