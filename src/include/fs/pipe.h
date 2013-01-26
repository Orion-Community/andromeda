/*
 *  Andromeda
 *  Copyright (C) 2012  Bart Kuivenhoven
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

/**
 * \defgroup Stream
 * @{
 */
#include <lib/tree.h>

#ifndef __FS_STREAM_H
#define __FS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct pipe_data_block
 * \var offset
 * \var next
 * \var prev
 * \var data
 */

struct pipe_data_block {
        unsigned int offset;

        struct pipe_data_block* next;
        struct pipe_data_block* prev;
        char data[1000];
};

struct pipe {
        int reading_idx;
        int writing_idx;

        int ref_cnt;

        void* data;

        int (*close)();
        int (*open)();

        int (*write)();
        int (*read)();
};

#ifdef __cplusplus
}
#endif

#endif

/**
 * @} \file
 */
