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

#define E_GENERIC               0x4
#define E_NORIGHTS              0x5
#define E_UNAUTHORISED          E_NORIGHTS
#define E_NOTFOUND              0x6
#define E_CORRUPT               0x7

#define E_PAGE_MAPPING          0x10
#define E_PAGE_NORIGHTS         0x11
#define E_PAGE_NOPAGE           0x12
#define E_PAGE_NOMEM            0x13

#define E_BMP_NOMAP             0x20
#define E_BMP_NOLIST            0x21
#define E_BMP_NOIDX             0x22
#define E_BMP_NOMEM             0x23
#define E_BMP_CORRUPT           0x24

#define E_FS_INIT               0x30
#define E_FS_SYNC               0x31
#define E_FS_UNKNOWN            0x32
#define E_FILE_NOFILE           0x33
#define E_FILE_NOBUFFER         0x34
#define E_FILE_COB              0x35 /* Cursor out of bounds */
#define E_FILE_NOSTREAM         0x36
#define E_STREAM_FAILURE        0x37

#define E_HEAP_GENERIC          0x40
#define E_NULL_PTR              0x41
#define E_ALREADY_INITIALISED   0x42
#define E_NOT_YET_INITIALISED   0x43

#define E_TASK_NOSPACE          0x50

#define E_INVALID_ARG           0x60
#define E_TOOLARGE_ARG          0x61
#define E_OUTOFBOUNDS           0x62
#define E_CONFLICT              0x63
#define E_NOT_FOUND             0x64

#define E_LOCKED                0x70


#define E_CLEAN_PARENT          0x200

#ifdef __cplusplus
}
#endif

#endif
