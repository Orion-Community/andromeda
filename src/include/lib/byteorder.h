/*
 *   Andromeda Project - Byte reorder functions.
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
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

#ifndef __BYTE_ORDER_H
#define __BYTE_ORDER_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned short __16be;
typedef unsigned int   __32be;

__16be htons(uint16_t);
__32be htonl(uint32_t);

unsigned short ntohs(__16be val);
unsigned int ntohl(__32be val);

#ifdef __cplusplus
}
#endif

#endif
