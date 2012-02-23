/*
 *   Andromeda Project - IRQ/ISR header.
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __IRQ_H
#define __IRQ_H
#include <arch/x86/irq.h>

#define IRQ(fn, arg1, arg2) \
static void fn(unsigned int arg1, irq_stack_t arg2); \
static void fn(unsigned int arg1, irq_stack_t arg2)
#endif
