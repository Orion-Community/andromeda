/*
    Andromeda
    Copyright (C) 2011, 2012  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <mm/map.h>
#include <thread.h>
#include <boot/mboot.h>
#include <stdlib.h>
#include <andromeda/error.h>
#include <andromeda/system.h>

#define map_size (memsize/0x4)

module_t modules[MAX_MODS];

struct page *page_map = NULL;
size_t memsize; // Size of memory in KiB

volatile mutex_t map_lock = mutex_unlocked;

int build_map(multiboot_memory_map_t* map, int mboot_map_size)
{
	page_map = kmalloc(map_size*sizeof(struct page));
	if(map == NULL)
		panic("No memory in build_map");
	int idx = 0;
	for (; idx < map_size; idx++)
	{
		page_map[idx].prev_idx = BMP_FREE;
		page_map[idx].next_idx = BMP_FREE;
	}
#ifdef PAGEDBG
	printf("Mem map size: %X B\tMem map size: %X B\n",
				    map_size*sizeof(struct page), memsize*1024);
#endif
	return -E_BMP_NOMAP;
}

/**
 * map_find_endoflist does what it says. DO NOT MAKE IT USE THE MUTEX as it is
 * supposed to be locked by higher level functions
 */

addr_t map_find_endoflist(addr_t idx)
{
	if (page_map[idx].next_idx == MAP_LAST_NODE &&
					page_map[idx].prev_idx == MAP_LAST_NODE)
		return idx;

	for (; page_map[idx].next_idx != BMP_FREE; idx = page_map[idx].next_idx)
	{
		if (page_map[idx].next_idx == BMP_FREE &&
					     page_map[idx].prev_idx == BMP_FREE)
			return -E_BMP_CORRUPT;
	}
	return idx;
}

/**
 * map_find_headoflist does what is says. DO NOT MAKE IT USE THE MUTEX as it is
 * supposed to be locked by higher level functions
 */
addr_t map_find_headoflist(addr_t idx)
{
	for (; page_map[idx].prev_idx != BMP_FREE; idx = page_map[idx].prev_idx)
	{
		if (page_map[idx].next_idx == BMP_FREE &&
                                               page_map[idx].prev_idx==BMP_FREE)
			return -E_BMP_CORRUPT;
	}
	return idx;
}

/**
 * map_add_page adds a page to the list you've specified. DO NOT MAKE IT USE THE
 * MUTEX as it is supposed to be locked by higher level functions
 */
int map_add_page(addr_t list_start, addr_t page_index)
{
	if (list_start >= map_size && list_start != MAP_NOMAP)
		return -E_BMP_NOIDX;
	else if (page_index >= map_size)
		return -E_BMP_NOIDX;
	else if (page_map == NULL)
		return -E_BMP_NOMAP;

	addr_t list_end = 0;
	if (list_start != MAP_NOMAP)
		list_end = map_find_endoflist(list_start);

	if (list_end == (addr_t)-E_BMP_CORRUPT)
		return -E_BMP_CORRUPT;

	page_map[list_end].next_idx = page_index;

	page_map[page_index].prev_idx = list_end;
	page_map[page_index].next_idx = BMP_FREE;

	addr_t ret = 0;
	if (list_start == MAP_NOMAP)
		ret = page_index;
	else
		ret = (addr_t)-E_SUCCESS;

	return ret;
}

/**
 * map_set_page adds a page to the list you've specified. This function does use
 * the mutex, since this is going to be called directly from other parts of the
 * OS. Do not use map_add_page if setting a page from outside of the sub-system.
 */
addr_t map_set_page(addr_t list_start, addr_t page_index)
{
	if (list_start >= map_size && list_start != MAP_NOMAP)
		return -E_BMP_NOIDX;
	else if (page_index >= map_size)
		return -E_BMP_NOIDX;
	else if (page_map == NULL)
		return -E_BMP_NOMAP;

	addr_t list_end = page_index;
	mutex_lock(&map_lock);
	if (list_start != MAP_NOMAP)
		list_end = map_find_endoflist(list_start);
	if (list_end == (addr_t)-E_BMP_CORRUPT)
	{
		mutex_unlock(&map_lock);
		return -E_BMP_CORRUPT;
	}
	if (list_end != page_index)
	{
		page_map[list_end].next_idx = page_index;

		page_map[page_index].prev_idx = list_end;
		page_map[page_index].next_idx = BMP_FREE;
	}
	else
	{
		page_map[list_end].next_idx = MAP_LAST_NODE;
		page_map[list_end].prev_idx = MAP_LAST_NODE;
	}
	mutex_unlock(&map_lock);

	addr_t ret = 0;
	if (list_start == MAP_NOMAP)
		ret = page_index;
	else
		ret = (addr_t)-E_SUCCESS;
	return ret;
}

/**
 * map_rm_page removes a page from the list and ALWAYS returns the head of the
 * list. Please keep the mutexes as granual as possible!
 */
addr_t map_rm_page(addr_t page_index)
{
	page_index = page_index >> 12;
	if (page_index >= map_size)
		return (addr_t)-E_BMP_NOIDX;
	else if (page_map == NULL)
		return (addr_t)-E_BMP_NOMAP;

	mutex_lock(&map_lock);

	addr_t prev_idx = page_map[page_index].prev_idx;
	addr_t next_idx = page_map[page_index].next_idx;

	if (prev_idx != BMP_FREE)
		page_map[prev_idx].next_idx = next_idx;
	if (next_idx != BMP_FREE)
		page_map[next_idx].prev_idx = prev_idx;

	page_map[page_index].prev_idx = BMP_FREE;
	page_map[page_index].next_idx = BMP_FREE;

	mutex_unlock(&map_lock);

	return map_find_headoflist(prev_idx);
}

/**
 * map_alloc_page allocates a page and adds it to the list. Please keep the
 * mutexes as granual as possible.
 */
addr_t map_alloc_page(addr_t list_idx)
{
	addr_t idx = 0;
	mutex_lock(&map_lock);
	for (; idx < map_size; idx++)
	{
		if (page_map[idx].next_idx == BMP_FREE &&
                                              page_map[idx].prev_idx==BMP_FREE)
		{
			map_add_page(list_idx, idx);
			mutex_unlock(&map_lock);
			return idx << 12;
		}
	}
	mutex_unlock(&map_lock);
	printf("Out of memory!\n");
	return (addr_t)-E_BMP_NOMEM;
}

/**
 * Debug function for looking through a memory map list.
 */
void map_show_list(addr_t list_idx)
{
	if (page_map[list_idx].next_idx == MAP_LAST_NODE)
	{
		printf("Only node: %X\n", list_idx);
		return;
	}
	int i = 1;
	int idx = page_map[list_idx].next_idx;
	printf("Node: %X\t IDX: %X\n", 0, list_idx);
	for (; idx != BMP_FREE; idx = page_map[idx].next_idx, i++)
	{
		printf("Node: %X\t IDX: %X\n", i, idx);
	}
}

/** \file */

