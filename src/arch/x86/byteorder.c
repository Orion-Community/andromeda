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

#include <stdlib.h>
#include <lib/byteorder.h>

/**
 * \fn htons(val)
 * \brief Converts a 16 bit host endian value to network endianess (big endian).
 * \param val Little endian input value
 */
__16be
htons(unsigned short val)
{
        unsigned short tmp = (val & 0xff) << 8;
        val >>= 8;
        return (val | tmp);
}

unsigned short
ntohs(__16be val)
{
        return htons(val);
}

__32be
htonl(unsigned int val)
{
        unsigned int tmp;
        unsigned int i = 0;
        for(; i < sizeof(unsigned int)/2; i++)
        {
               tmp = (val & 0xff) << 24;
               val >>= 8;
               val |= tmp;
        }
        return val;
}

unsigned int ntohl(__32be val)
{
        return htonl(val);
}
