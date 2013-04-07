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
#ifdef __INTEL

#ifndef __GDT_H
#define __GDT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FAST
// Here goes the GDT entry data structure
struct gdtEntry
{
   unsigned int limit_low   : 16;           // The lower 16 bits of the limit.
   unsigned int base_low    : 24;           // The lower 24 bits of the base address.
   unsigned int access      : 8;            // Access flags, determine what ring this segment can be used in.
   unsigned int granularity : 8;
   unsigned int base_high   : 8;            // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdtEntry gdtEntry_t;
#else
struct gdtEntry
{
  unsigned int limit		: 16; // Limit in bytes. Multiplied with 4096 if granularity is set.
  unsigned int baseLow		: 24; // The lower 24 bytes of the base address
  unsigned int type		: 4; // Indicates the type of segment.
  unsigned int s		: 1; // System flag
  unsigned int dpl		: 2; // Descriptor privilege level
  unsigned int one		: 1; // Needs to be one
  unsigned int limitHigh	: 4; // The rest of limit
  unsigned int avl		: 1; // For OS use.
  unsigned int zero		: 1; // Needs to be 0
  unsigned int mode		: 1; // 1 for 32 bits addresses
  unsigned int granularity	: 1; // 1 for multiplying the limit with 4096, to make it possible to span 4 GB
  unsigned int baseHigh		: 8; // The higher 8 bits of the base address
}__attribute__((packed));
typedef struct gdtEntry gdtEntry_t;
#endif

// Here comes the GDT pointer structure
struct gdtPtr
{
  unsigned int limit        : 16; // Indicate where the table ends in bytes minus 1
  unsigned int baseAddr     : 32; // Indicate where the table starts
} __attribute__((packed)); 	  // To tell the compiler that these fields should be packed together(1)
typedef struct gdtPtr gdt_t;
// The load gdt instruction
extern void lgdt(gdt_t*);

#endif

#ifdef __cplusplus
}
#endif

#endif
/*
(1) Compilers often do some form of optimalisation, and this can be done by giving each field it's
    own byte. We don't want this. We want something which is in the form that the CPU know what we
    want. That's why pack the bytes together.
*/