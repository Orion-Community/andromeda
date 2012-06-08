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
#include <thread.h>

int64_t atomic_add(atomic_t* d, int cnt)
{
        mutex_lock(&d->lock);
        d->cnt += cnt;
        int64_t ret = d->cnt;
        mutex_unlock(&d->lock);
        return ret;
}

uint64_t
atomic_set(atomic_t *atom)
{
        mutex_lock(&atom->lock);
        uint64_t ret = atom->cnt;
        atom->cnt = 1;
        mutex_unlock(&atom->lock);
        return ret;
}

uint64_t
atomic_reset(atomic_t *atom)
{
        mutex_lock(&atom->lock);
        uint64_t ret = atom->cnt;
        atom->cnt = 0;
        mutex_unlock(&atom->lock);
        return ret;
}

int64_t
atomic_sub(atomic_t* d, int cnt)
{
        return (atomic_add(d, -cnt));
}

int64_t
atomic_inc(atomic_t* d)
{
        return (atomic_add(d, 1));
}

int64_t
atomic_dec(atomic_t* d)
{
        return (atomic_add(d, -1));
}

int64_t
atomic_get(atomic_t* d)
{
        mutex_lock(&d->lock);
        int64_t ret = d->cnt;
        mutex_unlock(&d->lock);
        return ret;
}

int64_t
semaphore_inc(semaphore_t* s)
{
        boolean condition = FALSE;
        int64_t ret = 0;
        while (condition == FALSE)
        {
                mutex_lock(&s->lock);

                if (s->cnt < s->limit)
                {
                        s->cnt ++;
                        ret = s->cnt;
                        condition = TRUE;
                }
                mutex_unlock(&s->lock);
        }
        return ret;
}

int64_t
semaphore_dec(semaphore_t* s)
{
        boolean condition = FALSE;
        int64_t ret = 0;
        while (condition == FALSE)
        {
                mutex_lock(&s->lock);

                if (s->cnt > 0)
                {
                        s->cnt --;
                        ret = s->cnt;
                        condition = TRUE;
                }
                mutex_unlock(&s->lock);
        }
        return ret;
}