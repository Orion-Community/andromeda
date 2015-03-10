/*
 *  Andromeda
 *  Copyright (C) 2011 - 2015  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <types.h>

#ifndef __IOCTL_H
#define __IOCTL_H

typedef enum {
        IOCTL_UNDEFINED = 0x0,
        IOCTL_GET_PERMS = 0x1,
        IOCTL_SET_PERMS_OWNER = 0x2,
        IOCTL_SET_PERMS_GROUP = 0x3,
        IOCTL_SET_PERMS_WORLD = 0x4,

        IOCTL_SET_UID = 0x10,
        IOCTL_SET_GID = 0x11,
        IOCTL_GET_UID = 0x12,
        IOCTL_GET_GID = 0x13,

        IOCTL_GET_CACHE_LEN = 0x20,
        IOCTL_SET_CACHE_LEN = 0x21,

        IOCTL_VGA_TEXT_SET_FGCOLOUR = 0x80,
        IOCTL_VGA_TEXT_SET_FGCOLOR = IOCTL_VGA_TEXT_SET_FGCOLOUR,
        IOCTL_VGA_TEXT_SET_BGCOLOUR = 0x81,
        IOCTL_VGA_TEXT_SET_BGCOLOR = IOCTL_VGA_TEXT_SET_BGCOLOUR,
        IOCTL_VGA_TEXT_SET_CURSOR = 0x82,
        IOCTL_VGA_TEXT_GET_FGCOLOUR = 0x83,
        IOCTL_VGA_TEXT_GET_FGCOLOR = IOCTL_VGA_TEXT_GET_FGCOLOUR,
        IOCTL_VGA_TEXT_GET_BGCOLOUR = 0x84,
        IOCTL_VGA_TEXT_GET_BGCOLOR = IOCTL_VGA_TEXT_GET_BGCOLOUR,
        IOCTL_VGA_TEXT_GET_CURSOR = 0x85,

        IOCTL_UART_SET_BAUD = 0xA0,
        IOCTL_UART_SET_PARITY = 0xA1,
        IOCTL_UART_SET_STOPBIT = 0xA2,
        IOCTL_UART_SET_CHARLEN = 0xA3,
        IOCTL_UART_GET_BAUD = 0xB0,
        IOCTL_UART_GET_PARITY = 0xB1,
        IOCTL_UART_GET_STOPBIT = 0xB2,
        IOCTL_UART_GET_CHARLEN = 0xB3,

        IOCTL_TTY_RESIZE = 0x100,
        IOCTL_TTY_CONNECT_INPUT = 0x101,
        IOCTL_TTY_DISCONNECT_INPUT = 0x102,
        IOCTL_TTY_CONNECT_OUTPUT = 0x103,
        IOCTL_TTY_DISCONNECT_OUTPUT = 0x104,

        IOCTL_TTY_GET_SIZE = 0x110,
        IOCTL_TTY_GET_INPUT = 0x111,
        IOCTL_TTY_GET_OUTPUT = 0x112

} ioctl_t;

struct ioctl_vga_text_data {
        union {
                uint16_t colour;
                struct {
                        uint16_t height;
                        uint16_t widht;
                } cursor;
        };
};

struct ioctl_tty_data {
        union {
                size_t line_idx;
                struct {
                        uint16_t line_width;
                        uint16_t no_lines;
                } dimension;
                struct vfile* input_device;
                struct vfile* output_device;
        };
};

struct ioctl_serial_data {
        union {
                uint32_t baud;
                uint8_t parity;
                uint8_t charlen;
                uint8_t stopchar;
        };
};

#endif
