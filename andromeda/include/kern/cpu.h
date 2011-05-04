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

#ifndef __CPU_H
#define __CPU_H

extern int getVendor();
extern void sti();
extern void cli();
extern void halt();
#ifdef __INTEL
extern unsigned short getCS();
extern unsigned short getDS();
extern unsigned short getFS();
extern unsigned short getGS();
extern unsigned short getSS();
extern unsigned int getESP();
extern unsigned long getCR3();
extern unsigned void setCR3(long);
extern unsigned void toglePEbit();

extern boolean pebit;

#define VENDOR_INTEL	1
#define VENDOR_AMD	2
#endif
#endif