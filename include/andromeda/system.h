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

#ifndef __KERN_SYSTEM_H
#define __KERN_SYSTEM_H

#include <lib/tree.h>
#include <fs/vfs.h>
#include <types.h>
#ifdef X86
#include <boot/mboot.h>
#endif

#define CPU_LIMIT 0x10

struct sys_mmu_range {

};

struct sys_mmu {
        int (*set_page)(void* phys, void* virt);
        int (*reset_page)(void* virt);
        int (*get_phys)(void* virt);
        int (*get_virt)(void* phys);
        int (*set_range)(struct sys_mmu_range);
};

struct sys_cpu_scheduler {
        int (*sched)();
        int (*task_switch)();
        int (*get_load)();
        int (*set_task)();
        int (*unset_task)();
        int (*fork)();
        int (*signal)();
        int (*set_prio)();
};

struct sys_cpu_pic {
        int (*set_interrupt)();
        int (*get_interrupt)();
        int (*disable_interrupt)();
        int (*suspend)();
        int (*resume)();
};

struct sys_cpu {
        struct sys_mmu* mmu;
        struct sys_cpu_scheduler* scheduler;
        struct sys_cpu_pic* pic;

        int (*suspend)();
        int (*resume)();
        int (*halt)();
        int (*throttle)();
};

struct sys_io_pic {
        int (*set_interrupt)();
        int (*get_interrupt)();
        int (*disable_interrupt)();
        int (*suspend)();
        int (*resume)();
};

struct sys_arch_abstraction{
        struct sys_cpu* cpu[CPU_LIMIT];
        struct sys_io_pic* pic;
};

struct sys_memory_manager {
        void* (*page_alloc)();
        void* (*page_share)(void*);
        int (*page_free)(void*);
        void* (*alloc)(size_t, uint16_t);
        void  (*free)(void*, size_t);
};

struct sys_device_tree {
        int (*suspend)();
        int (*resume)();
        int (*get_dev)();
        int (*set_dev)();
        struct device* root_dev;
};

struct sys_module_tree {
        void* (*get_mod)();
        void* (*set_mod)();
};

struct sys_net {
        int (*send)(struct vfile* dev, char* data, int len);
        int (*recv)(struct vfile* dev, char* data, int len);
};

struct sys_fs {
        int (*mount)(struct vfile*, char* path);
        int (*umount)(char* path);
        int (*open)(char* path, char* rights);
        int (*close)(int file);
        int (*read)(int file, char* data, int len);
        int (*write)(int file, char* data, int len);
};

struct system {
        struct sys_arch_abstraction* arch;
        struct sys_memory_manager* mm;
        struct sys_device_tree* devices;
        struct sys_module_tree* kernel_modules;
        struct sys_fs* fs;
        struct sys_net* net;
};

extern struct system core;

#define hasmm (core.mm != NULL)

#define kmalloc(a) ((hasmm && core.mm->alloc != NULL) ? core.mm->alloc(a, 0) :\
                    NULL)
#define kfree(a) ((hasmm && core.mm->free != NULL) ?\
                   core.mm->free(a, sizeof(*a)) :\
                   panic("Memory freeing function not correctly initialised!"))
#define kfree_s(a,b) ((hasmm && core.mm->free != NULL) ?\
                    core.mm->free(a, b) :\
                    panic("Memory freeing function not correctly initialised!"))

#define phys_page_alloc ((hasmm && core.mm->page_alloc != NULL) ?\
                          core.mm->page_alloc() : NULL)
#define phys_page_share(a) ((hasmm && core.mm->page_share != NULL) ?\
                             core.mm->page_share(a) : NULL)
#define phys_page_free(a) ((hasmm && core.mm->page_free != NULL) ?\
                            core.mm->page_free(a) : 0)

int sys_setup_alloc();
int sys_setup_paging(multiboot_memory_map_t* map, unsigned int length);

#endif
