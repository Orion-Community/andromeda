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
 * \defgroup vmem
 * The virtual memory sub system
 * @{
 */

#ifndef __MM_X86_VMEM_H
#define __MM_X86_VMEM_H

#ifdef __cplusplus
extern "C" {
#endif

struct vmem_branch {
        union {
                struct vmem_branch* branches[16];
                void* virt_addr[16];
        };
};

int vmem_test_tree();

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 * \file
 */
