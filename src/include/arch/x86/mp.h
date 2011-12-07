/*
 *   The Andromeda project - MP Spec interface
 *   Copyright (C) 2011  Michel Megens
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

#ifndef __MP_H
#define __MP_H

struct mp_config_header
{
  
} __attribute((packed));

struct mp_fp_header
{
  uint32_t signature;
  struct mp_config_header* mp_header;
  uint8_t length, revision, checksum;
  uint8_t config_type; /* feature byte 1 */
  uint8_t imcrp; /* 
                      * if 1, the imcr is present
                      * and virtual wire mode is implemented
                      */
  //uint8_t reserved : 7;
  uint8_t reserved2, reserved3, reserved4; /* feature bytes 3-5 */
} __attribute((packed));

#endif
