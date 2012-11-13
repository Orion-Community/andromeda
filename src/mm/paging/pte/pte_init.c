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

#include <mm/pte.h>
#include <andromeda/error.h>
#include <stdlib.h>
#include <stdio.h>
#include <defines.h>

/**
 * \AddToGroup PTE
 */

/**
 * \todo Create statically allocated pte segments.
 * \todo Use existing page tables to attach to these segments.
 * \todo Grow the existing "asm" pagetables to cover the whole 3GB area.
 * \todo Create userspace pte initialiser.
 * \todo Create kernelspace pte initialiser.
 * \todo Create vmem context switcher.
 * \todo Rebuild practically the entire memory subsystem.
 */

/**
 * \var pte_core
 * \brief The pte tracker for the kernel
 */
struct pte_segment* pte_core[16];

// Initialise the pte system
int
pte_init(void* kernel_offset, size_t kernel_size)
{
        return -E_NOFUNCTION;
}
