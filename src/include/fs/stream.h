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

struct pipe_data_block {
        unsigned int tail;
        unsigned int head;
        struct pipe_data_block* next;
        struct pipe_data_block* prev;
        char data[1000];
};

struct pipe_data_list {
        struct pipe_data_block* head;
        struct pipe_data_block* tail;
};

struct stream {
        int reading;
        int writing;

        void* data;

        void (*open)();
        void (*close)();

        void (*input)();
        void (*output)();
};

/**
 * @} \file
 */
