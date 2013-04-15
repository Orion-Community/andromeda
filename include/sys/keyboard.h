/*
 *   Keyboard translation table.
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

#include <stdlib.h>

#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define OL_NUM_LED 0x2
#define OL_SCROLL_LED 0x1
#define OL_CAPS_LED 0x4

// kbc ports
#define OL_KBC_STATUS_REGISTER 0x64
#define OL_KBC_OUTPUT_BUFFER 0x60

#define OL_KBC_COMMAND_PORT 0x64
#define OL_KBC_DATA_PORT 0x60

void kb_handle(uint8_t c);
static void toggle_kb_leds(uint8_t);

typedef struct ol_kb_scancode {
	uint8_t xt;
	uint8_t at2;
	uint8_t at3;
	uint8_t sun;
	uint8_t usb;
	uint8_t adb;
	uint8_t code;
	uint8_t value;
        uint8_t capvalue;
} ol_kb_scancode_t;

#ifdef __cplusplus
}
#endif

#endif