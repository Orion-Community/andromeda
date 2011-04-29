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

#include <interrupts.h>
#include <interrupts/int.h>
#include <text.h>
#include <error/panic.h>
#include <PIC/PIC.h>
#include <APIC/APIC.h>

void initAPIC()
{
  pic = APIC;
  panic("No implementation for the APIC!");
}

void intInit()
{
  prepareIDT();
  #ifndef __COMPRESSED
  if (DetectAPIC())
  {
    #ifndef WARN
    initAPIC();
    #else
    printf("WARNING: The APIC hasn't got any implementation!\n");
    initPIC();
    #endif
    sti();
    return;
  }
  initPIC();
  sti();
  #endif
}
