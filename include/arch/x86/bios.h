/*
    Andromeda
    Copyright (C) 2014  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ARCH_X86_BIOS_H
#define __ARCH_X86_BIOS_H

struct Bios_Data_Area {
        uint16_t COM0;
        uint16_t COM1;
        uint16_t COM2;
        uint16_t COM3;

        uint16_t LPT0;
        uint16_t LPT1;
        uint16_t LPT2;

        uint16_t Extended_Bios_Data_Area;

        uint16_t packed_bit_flags;

        uint16_t kbd_state;
        uint8_t  kbd_buffer[32];

        uint8_t  display_mode;
        uint16_t text_cols;
        uint8_t  video_io_port[2];

        uint16_t timer_ticks;
        uint16_t hard_drives;

        uint16_t kbd_buffer_start;
        uint16_t kbd_buffer_end;

        uint8_t  shift_key_state;
};

extern struct Bios_Data_Area bda;

#endif
