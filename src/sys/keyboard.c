/*
 *   The implementation the of the keyboard header.
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
#include <interrupts/keyboard.h>
#include <stdlib.h>
#include <sys/io.h>

bool altkey = FALSE;
bool ctrlkey = FALSE;
bool endkey = FALSE;
bool shiftkey = FALSE;
bool capslock = FALSE;
uint8_t leds = 0;
void kb_handle(uint8_t c)
{
	if(c & 0x80)
	{
		/*
		 * Here I can test if the shift key is pressed or not and display 
                 * capital letters and stuff like that..
		 */
		uint8_t tmp = c ^ 0x80;
		if(tmp == 0x38) altkey = FALSE;
		if(tmp == 0x1d) ctrlkey = FALSE;
		if(tmp == 0x4f) endkey = FALSE;
                if(tmp == 0x2a) shiftkey ^= TRUE;
                if(tmp == 0x36) shiftkey ^= TRUE;
                if(tmp == 0x3a) shiftkey ^= TRUE;
                
                // togle keyboard leds
                if(tmp == 0x45) 
                {
                    leds ^= OL_NUM_LED;
                    toggle_kb_leds(leds);
                }
                if(tmp == 0x3a) 
                {
                    leds ^= OL_CAPS_LED;
                    toggle_kb_leds(leds);
                }
                if(tmp == 0x46) 
                {
                    leds ^= OL_SCROLL_LED;
                    toggle_kb_leds(leds);
                }
	}

	else
	{
		/*
		 * All default letters are taken from the array. All other keys 
                 * are handled in this switch case
		 */
		switch(c)
		{
                        case 0x2a:
                                shiftkey ^= TRUE;
                                break;
                        case 0x36:
                                shiftkey ^= TRUE;
                                break;
                                
			case 0xe0: // escaped scan codes
				break;
			case 0x38:
				altkey = TRUE;
				break;

			case 0x1d:
				ctrlkey = TRUE;
				break;

			case 0x4f:
				endkey = TRUE;
				break;

			case 0x50:
				scroll(1);
				break;
			default:
                                shiftkey ? putc(keycodes[c].capvalue) : 
                                    putc(keycodes[c].value);
				break;
		}
		
		if(ctrlkey && altkey && endkey)
		{
			reboot();
		}
	}
	return;
}

void toggle_kb_leds(uint8_t status)
{
    while((inb(OL_KBC_STATUS_REGISTER) & 0x2) != 0);
    //outb(OL_KBC_COMMAND_PORT, 0xed);
    outb(OL_KBC_DATA_PORT, 0xed);
    
    while((inb(OL_KBC_STATUS_REGISTER) & 0x2) != 0);
    outb(OL_KBC_DATA_PORT, status);
}
