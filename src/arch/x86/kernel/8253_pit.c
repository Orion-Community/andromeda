/*
 *   Copyright (C) 2015  Bart Kuivenhoven
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

#include <stdio.h>
#include <andromeda/system.h>

#define X86_8253_PIT_BASE               1193182

#define X86_8253_PIT_CHANNEL_0          0x40
#define X86_8253_PIT_CHANNEL_1          0x41
#define X86_8253_PIT_CHANNEL_2          0x42
#define X86_8253_PIT_COMMAND_PORT       0x43

#define X86_8253_PIT_COMMAND_CHANNEL_0          (0 << 6)
#define X86_8253_PIT_COMMAND_CHANNEL_1          (1 << 6)
#define X86_8253_PIT_COMMAND_CHANNEL_2          (2 << 6)
#define X86_8254_PIT_COMMAND_READBACK           (3 << 6)

#define X86_8253_PIT_COMMAND_MODE_CNT_DOWN      (0 << 1)
#define X86_8253_PIT_COMMAND_MODE_HW_RESET      (1 << 1)
#define X86_8253_PIT_COMMAND_MODE_RATE_GEN_1    (2 << 1)
#define X86_8253_PIT_COMMAND_MODE_RATE_GEN_2    (6 << 1)
#define X86_8253_PIT_COMMAND_MODE_SQUARE_1      (3 << 1)
#define X86_8253_PIT_COMMAND_MODE_SQUARE_2      (7 << 1)
#define X86_8253_PIT_COMMAND_MODE_SOFT_STROBE   (4 << 1)
#define X86_8253_PIT_COMMAND_MODE_HARD_STROBE   (5 << 1)

#define X86_8253_PIT_COMMAND_MODE_BINARY        (0 << 0)
#define X86_8253_PIT_COMMAND_MODE_BCD           (1 << 0)

#define X86_8253_PIT_COMMAND_ACCESS_LO          (1 << 4)
#define X86_8253_PIT_COMMAND_ACCESS_HI          (1 << 5)

int x86_pit_8253_init(int irq_no, time_t freq)
{
        int16_t divider = (int32_t)X86_8253_PIT_BASE / (int32_t)freq;

        uint8_t command = X86_8253_PIT_COMMAND_ACCESS_HI;
        command |= X86_8253_PIT_COMMAND_ACCESS_LO;
        command |= X86_8253_PIT_COMMAND_MODE_SQUARE_1;
        outb(X86_8253_PIT_COMMAND_PORT, command);

        uint8_t low = (uint8_t)(divider & 0xFF);
        uint8_t high = (uint8_t)(divider >> 8);

        outb(X86_8253_PIT_CHANNEL_0, low);
        outb(X86_8253_PIT_CHANNEL_0, high);

        andromeda_timer_init(freq, irq_no);

        return -E_SUCCESS;
}
