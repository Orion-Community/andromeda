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

#include <stdio.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <boot/mboot.h>

static struct mm_page_decriptor* pages = NULL;

/**
 * \fn mm_page_alloc
 * \brief Used to allocate pages
 * \param size
 * \brief The ammount of pages
 */

void*
mm_page_alloc(size_t size)
{
        return NULL;
}

/**
 * \fn mm_page_free
 * \brief Free the page previously allocated
 * \param page
 */
int
mm_page_free(void* page)
{
        return -E_NOFUNCTION;
}

/**
 * \fn mm_page_setup
 * \brief Build a list of available pages based on multiboot info
 * \param hdr
 * \brief The pointer to the multiboot data
 */
int
mm_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        if (pages != NULL)
                return -E_ALREADY_INITIALISED;



        return -E_NOFUNCTION;
}
