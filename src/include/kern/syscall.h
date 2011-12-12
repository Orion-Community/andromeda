/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#ifndef __KERN_SYSCALL_H
#define __KERN_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_INVALID     0x00

#define SYS_WRITE       0x01
#define SYS_READ        0x02
#define SYS_OPEN        0x03
#define SYS_CLOSE       0x04
#define SYS_SEEK        0x05

#define SYS_YIELD       0x10
#define SYS_FORK        0x11
#define SYS_KILL        0x12
#define SYS_SIG         0x13

#define SYS_BRK         0x20

#define SYS_SHUTDOWN    0x30
#define SYS_REBOOT      0x31
#define SYS_HIBERNATE   0x32
#define SYS_STANDBY     0x33

void syscall(int call_number, int arg1, int arg2, int arg3);

#ifdef __cplusplus
}
#endif

#endif
