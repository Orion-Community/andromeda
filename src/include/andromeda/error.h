/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ERROR_ERROR_H
#define __ERROR_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define E_SUCCESS               0x0

#define E_NOMEM                 0x1
#define E_NOFUNCTION            0x2
#define E_UNFINISHED            0x3

#define E_PAGE_MAPPING          0x4
#define E_PAGE_NORIGHTS         0x5
#define E_PAGE_NOPAGE           0x6
#define E_PAGE_NOMEM            0x7

#define E_BMP_NOMAP             0x8
#define E_BMP_NOLIST            0x9
#define E_BMP_NOIDX             0xA
#define E_BMP_NOMEM             0xB
#define E_BMP_CORRUPT           0xC

#define E_FS_INIT               0xD
#define E_FS_SYNC               0xE
#define E_FS_UNKNOWN            0xF
#define E_FILE_NOFILE           0x10
#define E_FILE_NOBUFFER         0x11
#define E_FILE_COB              0x12 /* Cursor out of bounds */
#define E_FILE_NOSTREAM         0x13
#define E_STREAM_FAILURE        0x14

#define E_HEAP_GENERIC          0x15
#define E_NULL_PTR              0x16
#define E_ALREADY_INITIALISED   0x17

#define E_TASK_NOSPACE          0x20

#define E_INVALID_ARG           0x30
#define E_TOOLARGE_ARG          0x31

#define E_GENERIC               0x100
#define E_NORIGHTS              0x101

#define E_CLEAN_PARENT          0x200

#ifdef __cplusplus
}
#endif

#endif
