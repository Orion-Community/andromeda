/*
 *   The OL standard library.
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

#ifndef __STDLIB_H
#define __STDLIB_H

#define NULL (void *)0  // the notorious null pointer
#define FALSE 0
#define TRUE !FALSE
#define OL_KBC_COMMAND 0x64
#define OL_KB_INIT 0xae

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned char bool;
typedef unsigned long size_t;

struct registers
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t esp;
} __attribute__((packed));
typedef struct registers *ol_registers_t;

struct segments
{
	uint16_t ds;
	uint16_t cs;
	uint16_t es;
	uint16_t fs;
	uint16_t gs;
	uint16_t ss;
	
} __attribute__((packed));
typedef struct segments *ol_segments_t;

/*
 * Halt the system until it is interrupted.
 */
extern void halt();
extern void endprogram();

/**
 * WARNING: This is NOT a safe reboot. It actually forces the system to crash 
 * (triple fault), which results in a CPU reset.
 */
extern void reboot();

extern ol_registers_t getregs();
extern ol_segments_t getsegs();
// extern void testIDT();
extern void setInterrupts();
extern void clearInterrupts();

#endif
