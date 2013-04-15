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

#ifndef STDLIB_H
#define STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define ANDROMEDA_VERSION "0.1.0"

/*
 * ANDROMEDA_MAGIC can be used to identify several data structures within the
 * kernel
 */
#define ANDROMEDA_MAGIC 0xc0debabe

#include <text.h>
#include <io.h>
#include <types.h>
#include <andromeda/panic.h>
#include <andromeda/error.h>
#include <mm/memory.h>
#include <math/math.h>
#include <debug.h>

#define BIT(x) (1 << (x))

struct list_node;


struct list{
        struct list_node* head;
        size_t size;
        mutex_t lock;
};

struct list_node {
        struct list_node* head;
        struct list_node* next;
        void* data;
};

struct registers
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t esp;
} __attribute__((packed));
typedef struct registers *and_registers_t;

struct segments
{
	uint16_t ds;
	uint16_t cs;
	uint16_t es;
	uint16_t fs;
	uint16_t gs;
	uint16_t ss;

} __attribute__((packed));
typedef struct segments *and_segments_t;

extern uint32_t higherhalf;
extern uint32_t rodata;
extern void start();
void reboot();
void shutdown();

/*
 * Default loops
 */

#define for_each_ll_entry_safe_count(head, carriage, tmp, i) \
                                                for(carriage = (head), tmp = \
                                                (head)->next; carriage != (NULL) && \
                                                carriage != (carriage)->next; \
                                                carriage = (tmp), tmp = \
                                                (carriage)->next, (i)++)

#define for_each_ll_entry_safe(head, carriage, tmp) \
                                                for(carriage = (head), tmp = \
                                                (head)->next; carriage != (NULL) && \
                                                carriage != (carriage)->next; \
                                                carriage = (tmp), tmp = \
                                                (carriage)->next)
#define for_each_ll_entry(head, carriage) for(carriage = (head); \
                                          carriage != (NULL); carriage = \
                                          (carriage)->next)

#define addressof(p) &(p)

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#define DECL_START extern "C" {
#else
#define DECL_START
#endif

#ifdef __cplusplus
#define DECL_END }
#else
#define DECL_END
#endif

#endif
