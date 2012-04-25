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
#include <mm/x86/vmem.h>
#include <mm/paging.h>
#include <andromeda/error.h>

static mutex_t vmem_lock = mutex_unlocked;

void vmem_alloc_init()
{
}

/**
 * \fn vmem_alloc
 * \brief Allocate virtual pages which are then mapped to physical ones
 * \param pages
 * \brief The number of pages requested in bytes
 * \return A pointer to the allocated region, NULL if failed
 * \todo Build the vmem_alloc function
 */
void*
vmem_alloc(size_t pages)
{
        if (pages == 0 || pages % PAGESIZE != 0)
                return NULL;
        mutex_lock(&vmem_lock);



        mutex_unlock(&vmem_lock);
        return NULL;
}

/**
 * \fn vmem_free
 * \brief Free the previously allocated pages
 * \param ptr
 * \brief A pointer to the region to be freed
 * \param pages
 * \brief The number of pages requested in bytes on alloc
 * \return An error code on success or failure
 */
int
vmem_free(void* ptr, size_t pages)
{
        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
