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
#include <interrupts/interrupts.h>
#include <interrupts/pic.h>
#include <interrupts/keyboard.h>
#include <interrupts/idt.h>
#include <stdlib.h>


uint64_t timer = 0;
uint64_t sleepTime = 0;
bool isSleeping = FALSE;
void cIRQ0(ol_irq_stack_t regs)
{
	if(isSleeping)
	{
		if(!(sleepTime == 0)) sleepTime--;
	}
	timer += 1;
	pic_eoi(0);
	
	return;
}

bool alt = FALSE;
bool ctrl = FALSE;
bool endkey = FALSE;
void cIRQ1(ol_irq_stack_t regs)
{
	uint8_t c = inb(0x60);

	if(c & 0x80)
	{
		/*
		 * Here I can test if the shift key is pressed or not and display capital letters and stuff like that..
		 */
		uint8_t tmp = c ^ 0x80;
		if(tmp == 0x38) alt = FALSE;
		if(tmp == 0x1d) ctrl = FALSE;
		if(tmp == 0x4f) endkey = FALSE;
	}

	else
	{
		/*
		 * All default letters are taken from the array. All other keys are handled in this switch case
		 */
		switch(c)
		{
			case 0xe0: // escaped scan codes
				break;
			case 0x38:
				alt = TRUE;
				break;

			case 0x1d:
				ctrl = TRUE;
				break;

			case 0x4f:
				endkey = TRUE;
				break;

			case 0x50:
				scroll(1);
				break;
			default:
				putc(keycodes[c].value);
				break;
		}
		
		if(ctrl && alt && endkey)
		{
			reboot();
		}
	}

	pic_eoi(1);
	return;
}

void cIRQ2(ol_irq_stack_t regs)
{
	pic_eoi(2);
	return;
}
void cIRQ3(ol_irq_stack_t regs)
{
	pic_eoi(3);
	return;
}
void cIRQ4(ol_irq_stack_t regs)
{
	pic_eoi(4);
	return;
}

void cIRQ5(ol_irq_stack_t regs)
{
	pic_eoi(5);
	return;
}
void cIRQ6(ol_irq_stack_t regs)
{
	pic_eoi(6);
	return;
}
void cIRQ7(ol_irq_stack_t regs)
{
	return;
}
void cIRQ8(ol_irq_stack_t regs)
{
	pic_eoi(8);
	return;
}
void cIRQ9(ol_irq_stack_t regs)
{
	putc('a');
	pic_eoi(9);
	return;
}
void cIRQ10(ol_irq_stack_t regs)
{
	pic_eoi(10);
	return;
}
void cIRQ11(ol_irq_stack_t regs)
{
	pic_eoi(11);
	return;
}
void cIRQ12(ol_irq_stack_t regs)
{
	pic_eoi(12);
	return;
}
void cIRQ13(ol_irq_stack_t regs)
{
	pic_eoi(13);
	return;
}
void cIRQ14(ol_irq_stack_t regs)
{
	putc('a');
	pic_eoi(14);
	return;
}
void cIRQ15(ol_irq_stack_t regs)
{
	putc('b');
	pic_eoi(15);
	return;
}