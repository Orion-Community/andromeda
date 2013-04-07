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

#ifndef __FS_PATH_H
#define __FS_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

struct __PATH_ELEMENT
{
  char name[255];
  struct __PATH_ELEMENT *next;
  uint8_t cursor;
};

void clean_path(struct __PATH_ELEMENT* elements);
struct __PATH_ELEMENT *parse_path(char* path);

#ifdef __cplusplus
}
#endif

#endif