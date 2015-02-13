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
#ifdef X86
#include <arch/x86/system.h>
#include <arch/x86/paging.h>
#endif

struct system core = {NULL, NULL, NULL, NULL, NULL, NULL};

int sys_setup_alloc()
{
        if (hasmm())
                panic("Memory allocation already initialised!");

#ifdef SLAB
        slab_alloc_init();
        core.mm = kmem_alloc(sizeof(*core.mm), 0);
        if (!hasmm())
                panic("The slab allocator was not initialised!");
        memset(core.mm, 0, sizeof(*core.mm));
        slab_sys_register();
#elif defined SLOB
        init_heap();
        complement_heap(&end, HEAPSIZE);
        core.mm = alloc(sizeof(*core.mm), 0);
        if (!hasmm())
                panic("The slob allocator failed!");
        memset(core.mm, 0, sizeof(*core.mm));
        slob_sys_register();
#endif

        return -E_SUCCESS;
}

#ifdef X86
int sys_setup_paging(multiboot_memory_map_t* map, unsigned int length)
#else
int sys_setup_paging()
#endif
{
        if (!hasmm())
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
        interrupt_init();
        if (hasarch())
                return -E_ALREADY_INITIALISED;
        if (!hasmm())
                return -E_NOT_YET_INITIALISED;

        core.arch = kmalloc(sizeof(*core.arch));
        if (!hasarch())
                panic("Out of memory! could not initialise arch!");
        memset(core.arch, 0, sizeof(*core.arch));

        int i = 0;
        for (; i < CPU_LIMIT; i++)
        {
#ifdef X86
                system_x86_cpu_init(i);
#endif
        }

#ifdef X86
        system_x86_init();
#endif

        return -E_SUCCESS;
}
int sys_setup_devices()
{
        if (core.devices != NULL)
                return -E_ALREADY_INITIALISED;
        warning("Device tree abstraction needs initialisation and code!\n");
        return -E_NOFUNCTION;
}
int sys_setup_modules()
{
        if (core.kernel_modules != NULL)
                return -E_ALREADY_INITIALISED;
        warning("Kernel module abstraction needs initialisation and code!\n");
        return -E_NOFUNCTION;
}
int sys_setup_fs()
{
        if (core.vfs != NULL)
                return -E_ALREADY_INITIALISED;

        core.vfs = kmalloc(sizeof(*core.vfs));
        if (core.vfs == NULL)
                panic("Unable to allocate the virtual filesystem data");

        memset (core.vfs, 0, sizeof(*core.vfs));

        vfs_init();

        warning("File system abstraction needs initialisation and code!\n");
        return -E_NOFUNCTION;
}
int sys_setup_net()
{
        if (core.net != NULL)
                return -E_ALREADY_INITIALISED;
        warning("Networking abstraction needs initialisation and code!\n");
        return -E_NOFUNCTION;
}
