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

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/buffer.h>

int buffer_write(struct buffer* this, char* buf, size_t num)
{
        warning("buffer_write Not yet implemented");
        return -E_NOFUNCTION;
}

int buffer_read(struct buffer* this, char* buf, size_t num)
{
        warning("buffer_read Not yet implemented");
        return -E_NOFUNCTION;
}

int buffer_close(struct buffer* this)
{
        warning("buffer_close Not yet implemented");
        return -E_NOFUNCTION;
}

struct buffer*
buffer_init()
{
        struct buffer* b = kalloc(sizeof(struct buffer));
        if (b == NULL)
                return NULL;
        memset(b, 0, sizeof(struct buffer));

        b->read = buffer_read;
        b->write = buffer_write;
        b->close = buffer_close;

        return b;
}