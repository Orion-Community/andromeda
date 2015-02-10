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

#ifndef __TEXT_H
#define __TEXT_H
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void println(char*);
void vprintf(char* fmt, va_list list);
void printf(char*, ...);
void putc(uint8_t c);
#define printnum(a,b,c,d) printNum(a,b,c,d)
void scroll(unsigned char);
void textInit();
int atoi(char* str);

#ifdef MSG_DBG
static inline void debug (char* fmt, ...) {
        va_list list;
        va_start (list, fmt);
        printf("[ DEBUG ] ");
        vprintf(fmt, list);
        va_end(list);
}
#else
static inline void debug(char* fmt __attribute__((unused)), ...)
{
}
#endif

#ifdef WARN
static inline void warning (char* fmt, ...)
{
        va_list list;
        va_start (list, fmt);

        printf("[ WARNING ] ");
        vprintf(fmt, list);

        va_end(list);
}
#else
static inline void warning (char* fmt __attribute__((unused)), ...)
{
}
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file */
