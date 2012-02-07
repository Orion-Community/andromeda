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

#ifndef __MM_CACHE_H
#define __MM_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CACHE_NAME_SIZE 255

struct slab {
        struct slab* next;
        void* data_ptr;
        void* page_ptr;
};

struct mm_cache {
        char name[CACHE_NAME_SIZE];
        struct slab* slabs_full;
        struct slab* slabs_partial;
        struct slab* slabs_empty;
        size_t obj_size;
        size_t pages;
};


#ifdef __cplusplus
}
#endif

#endif