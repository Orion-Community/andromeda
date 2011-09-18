/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Steven v.d. Schoot

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

#ifndef __TEXT_H
#define __TEXT_H

char* stdout;
char* stderr;

void textInit()
char* itoa(unsigned int index, char* buffer, unsigned int base)
char* dtoa(double index, char* buffer, unsigned int base)
void printf(const char *format, ...)
void fprintf(void *buffer, const char *format, ...)

#endif
