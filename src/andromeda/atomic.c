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
#include <andromeda/system.h>

int64_t atomic_add(atomic_t* d, int cnt)
{
        /* Make sure we can't be interrupted, (avoids interrupt blocking issues) */
        int intState = cpu_disable_interrupts(0);
        /* Make sure no other CPU can access this code */
        mutex_lock(&d->lock);
        /* Do the actual arithmetic */
        d->cnt += cnt;
        int64_t ret = d->cnt;
        /* Allow other CPU's */
        mutex_unlock(&d->lock);
        /* If interrupts were previously enabled, reenable */
        if (intState)
                cpu_enable_interrupts(0);
        return ret;
}

uint64_t atomic_set(atomic_t *atom)
{
        /* Avoid blocking issues with other interrupts! */
        int intState = cpu_disable_interrupts(0);
        /* Forbid other CPU's to touch our data */
        mutex_lock(&atom->lock);
        /* Set the bits! */
        uint64_t ret = atom->cnt;
        atom->cnt = 1;
        /* Allow touching of this memory again */
        mutex_unlock(&atom->lock);
        /* If interrupts were previously allowed, allow them again */
        if (intState)
                cpu_enable_interrupts(0);
        return ret;
}

uint64_t atomic_reset(atomic_t *atom)
{
        /* Atomic services, can I have your interrupts please? */
        int intState = cpu_disable_interrupts(0);
        /* I'll lock the door for you, please relax */
        mutex_lock(&atom->lock);
        /* Let's massage your bits to the correct value */
        uint64_t ret = atom->cnt;
        atom->cnt = 0;
        /* We may be disturbed again */
        mutex_unlock(&atom->lock);
        /* And here are your interrupts */
        if (intState)
                cpu_enable_interrupts(0);
        /* We hope to see you soon! */
        return ret;
}

int64_t atomic_sub(atomic_t* d, int cnt)
{
        return (atomic_add(d, -cnt));
}

int64_t atomic_inc(atomic_t* d)
{
        return (atomic_add(d, 1));
}

int64_t atomic_dec(atomic_t* d)
{
        return (atomic_add(d, -1));
}

int64_t atomic_get(atomic_t* d)
{
        /* Save and disable interrupt state */
        int intState = cpu_disable_interrupts(0);
        /* BLOCK!!! */
        mutex_lock(&d->lock);
        int64_t ret = d->cnt;
        /* UNBLOCK!!! */
        mutex_unlock(&d->lock);
        /* Return to previous interrupt state */
        if (intState)
                cpu_enable_interrupts(0);
        return ret;
}

void atomic_init(atomic_t* d, uint64_t cnt)
{
        if (d == NULL)
                return;
        d->cnt = cnt;
        d->lock = mutex_unlocked;
        return;
}

void semaphore_init(
                semaphore_t* s, uint64_t cnt, uint64_t lower_limit,
                uint64_t upper_limit)
{
        s->cnt = cnt;
        s->lower_limit = (int64_t)lower_limit;
        s->upper_limit = (int64_t)upper_limit;
        s->lock = mutex_unlocked;
}

int64_t semaphore_try_inc(semaphore_t* s)
{
        int64_t ret = 0;
        int intState;

        /* Fetch lock, no interrupts please */
        intState = cpu_disable_interrupts(0);
        int locked = mutex_test(&s->lock);
        if (locked == mutex_locked) {
                ret = -E_LOCKED;
                goto cleanup;
        }
        /* Try to do our job */
        if (s->cnt < s->upper_limit) {
                ret = s->cnt++;
        } else {
                ret = -E_OUT_OF_RESOURCES;
        }

        /* Apparently doing our job wasn't possible. Allow interrupts
         * and try again.
         */
        mutex_unlock(&s->lock);
        cleanup: if (intState)
                cpu_enable_interrupts(0);

        return ret;
}

int64_t semaphore_inc(semaphore_t* s)
{
        int64_t ret = 0;

        /* While we didn't complete our action */
        while (1) {
                ret = semaphore_try_inc(s);
                if (ret >= 0)
                        break;
                /**
                 * \todo Replace the iowait statement with a scheduler yield.
                 */
                iowait();
        }

        return ret;
}

int64_t semaphore_try_dec(semaphore_t* s)
{
        /* This code is similar to the above function, look there for comments */
        int64_t ret = 0;
        int intState;

        intState = cpu_disable_interrupts(0);
        int locked = mutex_test(&s->lock);
        if (locked == mutex_locked) {
                ret = -E_LOCKED;
                goto cleanup;
        }

        if (s->cnt > s->lower_limit) {
                ret = s->cnt--;
        } else {
                ret = -E_OUT_OF_RESOURCES;
        }

        mutex_unlock(&s->lock);

        cleanup: if (intState)
                cpu_enable_interrupts(0);

        return ret;
}

int64_t semaphore_dec(semaphore_t *s)
{
        int64_t ret = 0;
        while (1) {
                ret = semaphore_try_dec(s);
                if (ret >= 0)
                        break;
                iowait();
        }
        return ret;
}

int64_t semaphore_try_get(semaphore_t* s)
{
        int locked = mutex_test(&s->lock);
        if (locked == mutex_locked)
                return -E_LOCKED;

        int64_t ret = s->cnt;

        mutex_unlock(&s->lock);
        return ret;
}

int64_t semaphore_get(semaphore_t *s)
{
        while (1) {
                int64_t ret = semaphore_try_get(s);
                if (ret >= 0)
                        return ret;
        }
        /* Return only here to keep compiler happy */
        return -E_CORRUPT;
}
