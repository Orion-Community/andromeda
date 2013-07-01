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

/*
struct sys_mmu_range_phys {
        void* phys;
        size_t size;
        struct sys_mmu_range_phys* next;
};
*/

struct sys_mmu_range {
        /* virt is the base address of the range */
        void* virt;
        /* size length of the range (in number of pages) */
        size_t size;
        int cpl;
        int pid;
        //struct sys_mmu_range_phys* phys;
        void* arch_data;
};

struct sys_mmu {
        int (*set_page)(void* phys, void* virt, int privilege);
        int (*reset_page)(void* virt);
        void* (*get_phys)(void* virt);
        int (*set_range)(struct sys_mmu_range*);
        int (*reset_range)(struct sys_mmu_range*);
};

struct sys_cpu_scheduler {
        int (*sched)(void);
        int (*task_switch)(void);
        int (*get_load)(void);
        int (*set_task)(void);
        int (*unset_task)(void);
        int (*fork)(void);
        int (*signal)(void);
        int (*set_prio)(void);
};

struct sys_cpu_pic {
        int (*set_interrupt)(void);
        int (*get_interrupt)(void);
        int (*disable_interrupt)(void);
        int (*suspend)(void);
        int (*resume)(void);
};

struct sys_cpu {
        struct sys_mmu* mmu;
        struct sys_cpu_scheduler* scheduler;
        struct sys_cpu_pic* pic;

        void (*suspend)(void);
        void (*resume)(void);
        void (*halt)(void);
        void (*throttle)(void);
};

struct sys_io_pic {
        int (*set_interrupt)(void);
        int (*get_interrupt)(void);
        int (*disable_interrupt)(void);
        int (*suspend)(void);
        int (*resume)(void);
};

struct sys_arch_abstraction{
        struct sys_cpu* cpu[CPU_LIMIT];
        struct sys_io_pic* pic;
};

struct sys_memory_manager {
        void* (*page_alloc)(void);
        void* (*page_share)(void*);
        int (*page_free)(void*);
        void* (*alloc)(size_t, uint16_t);
        void  (*free)(void*, size_t);
};

struct sys_device_tree {
        int (*suspend)(void);
        int (*resume)(void);
        int (*get_dev)(void);
        int (*set_dev)(void);
        struct device* root_dev;
};

struct sys_module_tree {
        void* (*get_mod)(void);
        void* (*set_mod)(void);
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

#define hasmm() (core.mm != NULL)
#define hasarch() (hasmm() && core.arch != NULL)
#define hascpu(a) (hasarch() && a < CPU_LIMIT && core.arch->cpu[a] != NULL)

#define kmalloc(a) ((hasmm() && core.mm->alloc != NULL) ? core.mm->alloc(a, 0)\
                     : NULL)
#define kfree(a) ((hasmm() && core.mm->free != NULL) ?\
                   core.mm->free(a, sizeof(*a)) :\
                   panic("Memory freeing function not correctly initialised!"))
#define kfree_s(a,b) ((hasmm() && core.mm->free != NULL) ?\
                       core.mm->free(a, b) :\
                    panic("Memory freeing function not correctly initialised!"))

#define phys_page_alloc ((hasmm() && core.mm->page_alloc != NULL) ?\
                          core.mm->page_alloc() : NULL)
#define phys_page_share(a) ((hasmm() && core.mm->page_share != NULL) ?\
                             core.mm->page_share(a) : NULL)
#define phys_page_free(a) ((hasmm() && core.mm->page_free != NULL) ?\
                            core.mm->page_free(a) : 0)

#define getcpu(a) ((hasarch() && core.arch->cpu[a] != NULL && a < CPU_LIMIT) ?\
                    core.arch->cpu[a] : NULL)
#define setcpu(idx,ptr) ((hasarch() && idx < CPU_LIMIT) ?\
                          core.arch->cpu[idx] = ptr :\
                          panic("Setting cpu in invalid fashion"))

#define getmmu(a) ((getcpu(a) == NULL) ? getcpu(a)->mmu : NULL)

static inline void*
get_phys(int cpu, void* virt)
{
        if (!hascpu(cpu) || core.arch->cpu[cpu]->mmu == NULL)
                return NULL;
        return core.arch->cpu[cpu]->mmu->get_phys(virt);
}

static inline int
page_map(int cpu, void* virt, void* phys, int cpl)
{
        if (!hascpu(cpu))
                return -E_NULL_PTR;
        if (core.arch->cpu[cpu]->mmu == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->set_page(phys, virt, cpl);
}

static inline int
page_map_range(int cpu, struct sys_mmu_range* range)
{
        if (!hascpu(cpu) || range == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->set_range(range);
}

static inline int
page_unmap(int cpu, void* virt)
{
        if (!hascpu(cpu))
                return -E_NULL_PTR;
        if (core.arch->cpu[cpu]->mmu == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->reset_page(virt);
}

static inline int
page_unmap_range(int cpu, struct sys_mmu_range* range)
{
        if (!hascpu(cpu) || range == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->reset_range(range);
}

int sys_setup_alloc(void);
int sys_setup_devices(void);
int sys_setup_fs(void);
int sys_setup_arch(void);
int sys_setup_modules(void);
int sys_setup_net(void);

#ifdef X86
int sys_setup_paging(multiboot_memory_map_t* map, unsigned int length);
#else
int sys_setup_paging(void);
#endif

#endif

