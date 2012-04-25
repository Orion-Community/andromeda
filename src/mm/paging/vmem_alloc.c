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

/**
 * \todo build function that maps page tables into the page directory
 * \todo build function that maps pages into page tables
 * \todo build function that maps entries into the tree.
 */

/**
 * \fn vmem_map_pages
 */
int vmem_map_pages()
{
        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
