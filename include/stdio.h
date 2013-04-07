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
/**
 * \defgroup stdio
 * @{
 */

#ifndef __STDIO_H
#define __STDIO_H

#include <stdlib.h>
#include <thread.h>
#include <andromeda/buffer.h>
#include <io.h>

#ifdef __cplusplus
extern "C" {
#endif

int sprintf(char* str, char* fmt, ...);
int vsprintf(char* str, char* fmt, va_list list);
int vfprintf(struct vfile* stream, char* fmt, va_list list);
int fprintf(struct vfile* stream, char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

/** @} \file */
