/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \defgroup vmem
 * The virtual memory sub system
 * @{
 */
#ifndef __MM_VMEM_H
#define __MM_VMEM_H

#include <defines.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUDDY_NO_POWERS 13

/**
 * \struct vmem_branch
 * \brief Used in the physical to virtual translation
 */
struct vmem_branch {
        union {
                struct vmem_branch* branches[16];
                void* virt_addr[16];
        };
};

/**
 * \struct vmem_buddy
 * \brief Determines the condition a particular buddy resides in
 */
struct vmem_buddy {
        size_t size;
        void* ptr;
        struct vmem_buddy* next;
        struct vmem_buddy* prev;
        struct vmem_buddy_system* system;
};

/**
 * \struct vmem_buddy_system
 * \brief Defines the buddy system
 */
struct vmem_buddy_system {
        /**
         * \var system_size
         * \var buddies
         * \brief This an array of lists of buddies, sorted by size
         * \brief entry 0 being 1KiB, 8 being 256KiB
         * \var allocated
         */
        struct vmem_buddy* buddies[BUDDY_NO_POWERS];
        struct vmem_buddy* allocated;
        struct vmem_buddy_system* next;
        struct vmem_buddy_system* prev;
        bool full;
};

/**
* \fn vmem_test_tree
* \brief A unit test for the vmem lookup tree
* \return A generic error code
*/
int vmem_test_tree();
int vmem_init();
void vmem_alloc_init();
struct vmem_buddy_system* vmem_buddy_system_init(void* base_ptr, size_t size);
void* vmem_buddy_system_alloc(struct vmem_buddy_system* system, size_t size);
void vmem_buddy_system_free(struct vmem_buddy_system* system, void* ptr);
int vmem_buddy_test();
#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 * \file
 */
