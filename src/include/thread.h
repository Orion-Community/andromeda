/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

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

#include <stdlib.h>

#ifndef __THREAD_H
#define __THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#define mutex_lock mutexEnter
#define mutex_test mutexTest
#define mutex_unlock mutexRelease

#define mutex_locked 1
#define mutex_unlocked 0

typedef unsigned int mutex_t;

typedef struct {
        int64_t cnt;
        mutex_t lock;
} atomic_t;

typedef struct {
        int64_t cnt;
        mutex_t lock;
        int64_t limit;
}semaphore_t;

extern void mutexEnter(mutex_t);
extern unsigned int mutexTest(mutex_t);
extern void mutexRelease(mutex_t);

int64_t atomic_add(atomic_t* d, int cnt);
int64_t atomic_sub(atomic_t* d, int cnt);
uint64_t atomic_set(atomic_t *atom);
uint64_t atomic_reset(atomic_t *atom);
int64_t atomic_inc(atomic_t* d);
int64_t atomic_dec(atomic_t* d);
int64_t atomic_get(atomic_t* d);


#ifdef __cplusplus
}
#endif

#endif
