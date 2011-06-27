/*
 *   PIC header
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

#include <sys/stdlib.h>

#ifndef __H_PIC
#define __H_PIC
void pic_remap(uint32_t set1, uint32_t set2);
void pic_eoi(uint8_t irq);
void pic_init();

#define GEBL_INTERRUPT_BASE 0x20

#define GEBL_PIC1_COMMAND 0x20
#define GEBL_PIC2_COMMAND 0xa0

#define GEBL_PIC1_DATA 0x21
#define GEBL_PIC2_DATA 0xa1

// A few pic commands

#define GEBL_PIC_EOI 0x20	/* end of interrupt */
#define GEBL_ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define GEBL_ICW1_SINGLE 0x02	/* Single (cascade) mode */
#define GEBL_ICW1_INTERVAL4 0x04	/* Call address interval 4 (8) */
#define GEBL_ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define GEBL_ICW1_INIT 0x10		/* Initialization - required! */
 
#define GEBL_ICW4_8086 0x01		/* 8086/88 (MCS-80/85) mode */
#define GEBL_ICW4_AUTO 0x02		/* Auto (normal) EOI */
#define GEBL_ICW4_BUF_SLAVE 0x08	/* Buffered mode/slave */
#define GEBL_ICW4_BUF_MASTER 0x0C	/* Buffered mode/master */
#define GEBL_ICW4_SFNM 0x10		/* Special fully nested (not) */
#define GEBL_ICW3_MASTER 0x4
#define GEBL_ICW3_SLAVE 0x2


#endif