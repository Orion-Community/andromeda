/*
 *   The interrupts
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

#include <textio.h>
#include "include/interrupts.h"
#include "include/pic.h"
#include "include/keyboard.h"


uint64_t timer = 0;
void cIRQ0(gebl_isr_stack regs)
{
	timer += 1;
	pic_eoi(0);
	
// 	int mod = timer % 10;
// 	printnum(timer, 10, FALSE, FALSE);
// 	putc(0xa);
	
	return;
}
void cIRQ1(gebl_isr_stack regs)
{
	char c = inb(0x60);
	if(c & 0x80)
	{}
	else putc(keycodes[c].value);

	pic_eoi(1);
	return;
}

void cIRQ2(gebl_isr_stack regs)
{
	pic_eoi(2);
	return;
}
void cIRQ3(gebl_isr_stack regs)
{
	pic_eoi(3);
	return;
}
void cIRQ4(gebl_isr_stack regs)
{
	pic_eoi(4);
	return;
}

void cIRQ5(gebl_isr_stack regs)
{
	pic_eoi(5);
	return;
}
void cIRQ6(gebl_isr_stack regs)
{
	pic_eoi(6);
	return;
}
void cIRQ7(gebl_isr_stack regs)
{
	return;
}
void cIRQ8(gebl_isr_stack regs)
{
	pic_eoi(8);
	return;
}
void cIRQ9(gebl_isr_stack regs)
{
	putc('a');
	pic_eoi(9);
	return;
}
void cIRQ10(gebl_isr_stack regs)
{
	pic_eoi(10);
	return;
}
void cIRQ11(gebl_isr_stack regs)
{
	pic_eoi(11);
	return;
}
void cIRQ12(gebl_isr_stack regs)
{
	pic_eoi(12);
	return;
}
void cIRQ13(gebl_isr_stack regs)
{
	pic_eoi(13);
	return;
}
void cIRQ14(gebl_isr_stack regs)
{
	pic_eoi(14);
	return;
}
void cIRQ15(gebl_isr_stack regs)
{
	pic_eoi(15);
	return;
}