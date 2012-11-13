/*
 *  Orion OS, The educational operatingsystem
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
#include <andromeda/error.h>
#include <mm/page_alloc.h>

int pagemap[PAGE_LIST_SIZE];
int first_free = 1;

int page_alloc_init()
{
        int i = 0;
        while (i < PAGE_LIST_SIZE)
        {
                pagemap[i] = i++;
        }

        pagemap[PAGE_LIST_SIZE-1] = PAGE_LIST_ALLOCATED;
        pagemap[0] = PAGE_LIST_ALLOCATED;

        /**
         * Parse the grub memory map here to mark all the unusable pages as
         * Allocated
         */



        return -E_NOFUNCTION;
}
