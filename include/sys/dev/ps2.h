/*
 *   OpenLoader - PS/2 Controller
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

#ifndef __PS2_H
#define	__PS2_H

#ifdef __cplusplus
extern "C" {
#endif

/* device type */
#define OL_PS2_CONTROLLER 0x1
#define OL_PS2_KEYBOARD 0x2
#define OL_PS2_MOUSE 0x4

/* commands */
#define OL_PS2_INIT_KB_CMD 0xae

/* PS/2 I/O ports */
/* write only ports */
#define OL_PS2_DATA_PORT 0x60
#define OL_PS2_COMMAND_PORT 0x64

/* read only ports*/
#define OL_PS2_STATUS_REGISTER 0x64
#define OL_PS2_OUTPUT_BUFFER 0x60

/* flags */
#define OL_PS2_ACTIVE_FLAG 0x1 /* this marks the device as initialized and reade
                                  for action */
#define OL_PS2_ERROR_FLAG 0x2 /* indicates an error */

/* data types */
typedef unsigned char ol_ps2_dev_type_t;
typedef struct ol_ps2_dev
{
        ol_ps2_dev_type_t type;
        uint8_t flags;
        uint8_t status;
        uint16_t read_port;
        uint16_t write_port;
        void (*sent_command) (struct ol_ps2_dev*, uint8_t);

} *ol_ps2_dev_t;

#if 0
static void
ol_ps2_update_status(ol_ps2_dev_t);

static int
ol_ps2_init_dev(ol_ps2_dev_t, ol_ps2_dev_type_t);

static uint8_t
ol_ps2_await_ack(ol_ps2_dev_t);

static void
ol_ps2_sent_controller_command(ol_ps2_dev_t ctrl, uint8_t cmd);

static uint8_t
ol_ps2_read(ol_ps2_dev_t);

static int
ol_ps2_write(ol_ps2_dev_t, uint8_t, bool);
#endif
int
ol_ps2_init_keyboard();

void
ol_detach_all_devices();

int
ol_ps2_config_keyboard(uint8_t confval);

#ifdef __cplusplus
}
#endif

#endif	/* __PS2_H */

