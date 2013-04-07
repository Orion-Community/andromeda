/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Michel Megens, Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file file */

#include <stdlib.h>

#ifndef __THREAD_H
#define __THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#define mutex_locked 1
#define mutex_unlocked 0

/**
 * \def THREAD
 * \brief Definition of a thread handler.
 * \param fn The function name.
 * \param argv The name of the function argument pointer.
 *
 * This thread defines the header of a runnable thread. All runnable threads are
 * <i><b>static void</b></i> functions and they take one <i><b>void pointer</b>
 * </i> as argument.
 */
#define THREAD(fn, argv) \
static void fn(void *argv); \
static void fn(void *argv)

/**
 * \fn thread_t
 * \brief Thread function pointer.
 * \param argv Arguments which are passed to the thread.
 *
 * This function pointer is used to call the thread from the scheduler.
 */
typedef void (*thread_t)(void *argv);

typedef volatile unsigned int spinlock_t;
#define mutex_t spinlock_t

typedef volatile struct {
        int64_t cnt;
        spinlock_t lock;
} atomic_t;

typedef volatile struct {
        uint64_t cnt;
        spinlock_t lock;
        uint64_t limit;
}semaphore_t;

extern void mutex_lock(spinlock_t*);
extern unsigned int mutex_test(spinlock_t*);
extern void mutex_unlock(spinlock_t*);

int64_t atomic_add(atomic_t* d, int cnt);
int64_t atomic_sub(atomic_t* d, int cnt);
uint64_t atomic_set(atomic_t *atom);
uint64_t atomic_reset(atomic_t *atom);
int64_t atomic_inc(atomic_t* d);
int64_t atomic_dec(atomic_t* d);
int64_t atomic_get(atomic_t* d);
int64_t semaphore_inc(semaphore_t* s);
int64_t semaphore_dec(semaphore_t* s);

#ifdef __cplusplus
}
#endif

#endif
