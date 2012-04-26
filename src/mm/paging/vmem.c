/*
 * Andromeda
 * Copyright (C) 2011  Bart Kuivenhoven
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
 * \AddToGroup vmem
 * @{
 */
#include <mm/vmem.h>
#include <mm/paging.h>
#include <andromeda/error.h>

struct vmem_branch* lookup_tree;

/**
 * \fn vmem_init
 * \brief Initialise the virtual memory subsystem
 */
int
vmem_init()
{
        if (lookup_tree != NULL)
                return -E_ALREADY_INITIALISED;
        lookup_tree = kalloc(sizeof(*lookup_tree));

        return -E_SUCCESS;
}

/**
 * \fn vmem_branch_init
 * \brief Create and initialise a new branch
 * \return ptr to new branch
 */
struct vmem_branch*
vmem_branch_init()
{
        struct vmem_branch* ret = kalloc(sizeof(*ret));
        if (ret == NULL)
                return NULL;

        memset(ret, 0, sizeof(*ret));
        return ret;
}

#define MAP_DEPTH 4
#define FOUR_BITS 0xF
#define TWELVE_BITS 0x3FF

/**
 * \fn vmem_map
 * \brief Map a physical page to a virtual location
 * \param phys
 * \param virt
 * \return General error code
 */
int
vmem_map(void* phys, void* virt)
{
        /** Check the arguments */
        if (phys == NULL || virt == NULL)
                return -E_NULL_PTR;

        if (lookup_tree == NULL)
                return -E_NOT_YET_INITIALISED;

        if ((addr_t)phys & TWELVE_BITS != 0 || (addr_t)virt & TWELVE_BITS != 0)
                return -E_INVALID_ARG;

        /** Now find the branch to work with */
        register int i = 0;
        register addr_t phys_addr = (addr_t)phys;
        phys_addr >>= 12;
        register struct vmem_branch* carriage = lookup_tree;
        register int branch_idx;
        for (; i < MAP_DEPTH; i++)
        {
                branch_idx = phys_addr & FOUR_BITS;
                phys_addr >>= 4;
                if (carriage->branches[branch_idx] == NULL)
                        carriage->branches[branch_idx] = vmem_branch_init();
                if (carriage->branches[branch_idx] == NULL)
                        return -E_NOMEM;
                carriage = carriage->branches[branch_idx];
        }

        /** Now finally set the leaf ... */
        branch_idx = phys_addr & FOUR_BITS;
        carriage->virt_addr[branch_idx] = virt;

        return -E_SUCCESS;
}

/**
 * \fn vmem_map_region
 * \brief Map a region of physical addresses to linear ones
 * \param phys
 * \param virt
 * \param size
 * \brief The ammount of bytes to map, must be a multiple of PAGESIZE
 * \return A generic error code
 */
int vmem_map_region(void* phys, void* virt, size_t size)
{
        if (phys == NULL || virt == NULL)
                return -E_NULL_PTR;
        if (lookup_tree == NULL)
                return -E_NOT_YET_INITIALISED;
        if (((addr_t)phys & TWELVE_BITS) || ((addr_t)virt & TWELVE_BITS) ||
                                                          (size & TWELVE_BITS))
                return -E_INVALID_ARG;

        register int idx = 0;
        for (; idx < size; idx += PAGESIZE)
        {
                register int ret = vmem_map((void*)((addr_t)phys+idx),
                                                     (void*)((addr_t)virt+idx));
                if (ret != -E_SUCCESS)
                        return ret;
        }
        return -E_SUCCESS;
}

/**
 * \fn phys_to_virt
 * \brief Translate a physical address to a linear one
 * \param phys
 * \return The linear address corresponding to the physical one
 */
void*
phys_to_virt(void* phys)
{
        /** Check some preconditions */
        if (phys == NULL)
                return NULL;
        if (lookup_tree == NULL)
                return NULL;

        /** Find the right branch */
        register int i = 0;
        addr_t phys_addr = (addr_t)phys >> 12;
        register int branch_idx;
        struct vmem_branch* carriage = lookup_tree;
        for (; i < MAP_DEPTH; i++)
        {
                branch_idx = phys_addr & FOUR_BITS;
                phys_addr >>= 4;
                if (carriage->branches[branch_idx] == NULL)
                        return NULL;
                carriage = carriage->branches[branch_idx];
        }

        /** Return the right leaf */
        branch_idx = phys_addr & FOUR_BITS;
        return carriage->virt_addr[branch_idx];
}

/**
 * \fn vmem_test_tree
 * \brief A unit test for the vmem lookup tree
 * \return A generic error code
 */
int
vmem_test_tree()
{
        printf("Testing the vmem lookup tree\n");
        if (lookup_tree == NULL)
        {
                printf("TEST FAILED!\n");
                printf("The lookup tree hasn't been initialised propperly!\n");
                printf("Initialising\n");
                vmem_init();
                if (lookup_tree == NULL)
                {
                        printf("Couldn't initialise the lookup tree\n");
                        return -E_NOT_YET_INITIALISED;
                }
        }
        int ret = vmem_map_region((void*)0x1000, (void*)0xC0001000, 0x4000);
        if (ret != -E_SUCCESS)
        {
                printf("TEST FAILED!\n");
                printf("Could not map the requested region\n");
                printf("Error code: %X\n", -ret);
                return ret;
        }
        void* tst = phys_to_virt((void*)0x1000);
        if (tst != (void*)0xC0001000)
        {
                printf("TEST FAILED!\n");
                printf("Translating the physical address returned:  %X\n",(int)tst);
                return -E_GENERIC;
        }
        tst = phys_to_virt((void*)0x2000);
        if (tst != (void*)0xC0002000)
        {
                printf("TEST FAILED!\n");
                printf("Translating the physical address returned:  %X\n", (int)tst);
                return -E_GENERIC;
        }
        printf("Test completed successfully!\n\n");
        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
