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

#include <stdio.h>
#include <stdlib.h>
#include <fs/pipe.h>

/**
 * \AddToGroup Stream
 * @{
 */


static int pipe_read()
{
        return -E_NOFUNCTION;
}

static int pipe_write()
{
        return -E_NOFUNCTION;
}

static int pipe_close()
{
        return -E_NOFUNCTION;
}

static int pipe_open()
{
        return -E_NOFUNCTION;
}

struct pipe* stream_new()
{
        struct pipe* p = kalloc(sizeof(*p));
        if (p == NULL)
                return NULL;

        memset(p, 0, sizeof(*p));

        p->read = pipe_read;
        p->write = pipe_write;
        p->close = pipe_close;
        p->open = pipe_open;

        return NULL;
}

/**
 * @} \file
 */
