/*
 *  Andromeda
 *  Copyright (C) 2013  Bart Kuivenhoven
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

#include <andromeda/system.h>
#include <andromeda/error.h>
#include <mm/page_alloc.h>
#include <types.h>
#ifdef SLAB
#include <mm/cache.h>
#elif defined SLOB
#include <mm/heap.h>
#include <mm/memory.h>
#endif

struct system core = {NULL, NULL, NULL, NULL, NULL, NULL};

int sys_setup_alloc()
{
        if (core.mm != NULL)
                panic("Memory allocation already initialised!");

#ifdef SLAB
        slab_alloc_init();
        core.mm = kmem_alloc(sizeof(*core.mm), 0);
        if (core.mm == NULL)
                panic("The slab allocator was not initialised!");
        memset(core.mm, 0, sizeof(*core.mm));
        slab_sys_register();
#elif defined SLOB
        init_heap();
        complement_heap(&end, HEAPSIZE);
        core.mm = alloc(sizeof(*core.mm), 0);
        if (core.mm == NULL)
                panic("The slob allocator failed!");
        memset(core.mm, 0, sizeof(*core.mm));
        slob_sys_register();
#endif

        return -E_SUCCESS;
}

int sys_setup_paging(multiboot_memory_map_t* map, unsigned int length)
{
        if (core.mm == NULL)
                panic("Memory allocation not initialised!");

#ifdef X86
        x86_pte_init();
        page_alloc_init(map, length);
#endif
        page_alloc_register();
        return -E_SUCCESS;
}

int sys_setup_arch()
{
        if (core.arch != NULL)
                return -E_ALREADY_INITIALISED;
        return -E_NOFUNCTION;
}
int sys_setup_devices()
{
        if (core.devices != NULL)
                return -E_ALREADY_INITIALISED;
        return -E_NOFUNCTION;
}
int sys_setup_modules()
{
        if (core.kernel_modules != NULL)
                return -E_ALREADY_INITIALISED;
        return -E_NOFUNCTION;
}
int sys_setup_fs()
{
        if (core.fs != NULL)
                return -E_ALREADY_INITIALISED;
        return -E_NOFUNCTION;
}
int sys_setup_net()
{
        if (core.net != NULL)
                return -E_ALREADY_INITIALISED;
        return -E_NOFUNCTION;
}
