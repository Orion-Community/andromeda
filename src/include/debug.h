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

#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNDEFINED
#undef UNDEFINED
#endif // UNDEFINED

#ifdef DBG

#ifndef PAGEDBG
#define PAGEDBG
#endif // PAGEDBG

#ifndef __DBG_HEAP
#define __DBG_HEAP
#endif // __DBG_HEAP

#ifndef __PCI_DEBUG
#define __PCI_DEBUG
#endif // __PCI_DEBUG

#ifndef __CPU_DEBUG
#define __CPU_DEBUG
#endif // __CPU_DEBUG

#ifndef __IOAPIC_DBG
#define __IOAPIC_DBG
#endif //__IOAPIC_DBG

#ifndef STREAM_DBG
#define STREAM_DBG
#endif //STREAM_DBG

#define assert(a) if (!a) {panic("Assertion failed!");}
#else
#define assert(a)

#endif // DBG

#ifdef __cplusplus
}
#endif

#endif // __DEBUG_H

/** \file */
