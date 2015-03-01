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
#include <arch/x86/cpu.h>
#endif

#include <mm/memory.h>
#ifdef SLAB
#include <mm/cache.h>
#else
#include <mm/heap.h>
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
        int (*cleanup_range)(struct sys_mmu_range*);
};

typedef int (*handler)(int16_t timer_id, time_t time, int16_t irq_id);

struct sys_timer {
        int (*subscribe)(time_t time, uint16_t id, handler call_back,
                        struct sys_timer* timer);
        int (*set_freq)(time_t freq, struct sys_timer* timer);

        struct tree_root* events;

        time_t freq;
        volatile time_t time;
        atomic_t tick;

        uint16_t interrupt_id;

        mutex_t timer_lock;
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
        struct sys_timer* timers;
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
        int (*disable_interrupt)(void);
        int (*enable_interrupt)(void);
};

struct sys_io_pic {
        struct tree_root* timers;
        int (*set_interrupt)(void);
        int (*get_interrupt)(void);
        int (*disable_interrupt)(void);
        int (*suspend)(void);
        int (*resume)(void);
};

struct sys_arch_abstraction {
        struct sys_cpu* cpu[CPU_LIMIT];
        struct sys_io_pic* pic;
};

struct sys_memory_manager {
        void* (*page_alloc)(void);
        void* (*page_share)(void*);
        int (*page_free)(void*);
        void* (*alloc)(size_t, uint16_t);
        void (*free)(void*, size_t);
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

struct sys_fs_dir_entry {
        char name[256];
        uint32_t inode;
};

struct sys_fs {
        struct vfile* (*open)(int inode);
        struct sys_fs_dir_entry* (*dir_entry)(uint32_t inode, uint32_t entry,
                        char* data, size_t len);
        int (*close)(struct vfile*);
        int (*read)(struct vfile* file, char* data, size_t len);
        int (*write)(struct vfile* file, char* data, size_t len);
};

struct sys_vfs {
        int (*mount)(struct sys_fs*, char* path);
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
        struct sys_vfs* vfs;
        struct sys_net* net;
};

extern struct system core;

struct sys_timer* get_global_timer(int16_t irq_no);
struct sys_timer* get_cpu_timer(int16_t cpu);

#define hasmm() (core.mm != NULL)
#define hasarch() (hasmm() && core.arch != NULL)
#define hascpu(a) (hasarch() && a < CPU_LIMIT && core.arch->cpu[a] != NULL)

/**
 * \fn kmalloc
 * \brief Allocate the specified memory size
 * \param a
 */
#define kmalloc(a) ((hasmm() && core.mm->alloc != NULL && a > 1) ? core.mm->alloc(a, 0)\
                     : NULL)
/**
 * \fn kfree
 * \brief Free the pointer
 * \warning This might fail on arrays and types that don't contain object size
 * \param a
 */
#define kfree(a) ((core.mm->free != NULL) ?\
                   core.mm->free(a, sizeof(*a)) :\
                   panic("Memory freeing function not correctly initialised!"))

/**
 * \fn kfree_s
 * \brief Free the pointer using size information
 * \warning If no size information is provided, use this function to specify manually
 * \param a
 * \param b
 */
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

#define getarch() (core.arch)
#define getIOPIC() (core.arch->pic)
#define hasIOPIC() (getIOPIC() != NULL)

static inline time_t getTime(struct sys_timer* timer)
{
        if (timer == NULL) {
                return -1;
        }
        return timer->time;
}

static inline int subscribe_global_timer(int16_t irq_no, time_t time,
                int16_t id, handler call_back)
{
        struct sys_timer* timer = get_global_timer(irq_no);
        if (timer != NULL) {
                return timer->subscribe(time, id, call_back, timer);
        }
        return -E_NOTFOUND;
}

static inline int subscribe_global_timer_offset(int16_t irq_no, time_t offset,
                int16_t id, handler call_back)
{
        struct sys_timer* timer = get_global_timer(irq_no);
        if (timer != NULL) {
                time_t time = getTime(timer);
                time += offset;
                return timer->subscribe(time, id, call_back, timer);
        }
        return -E_NOTFOUND;
}

static inline int subscribe_cpu_timer(int16_t cpu_no, time_t time, int16_t id,
                handler call_back)
{
        struct sys_timer* timer = get_cpu_timer(cpu_no);
        if (timer != NULL) {
                return timer->subscribe(time, id, call_back, timer);
        }
        return -E_NOTFOUND;
}

static inline int subscribe_cpu_timer_offset(int16_t cpu_id, time_t offset,
                int16_t id, handler call_back)
{
        struct sys_timer* timer = get_cpu_timer(cpu_id);
        if (timer != NULL) {
                time_t time = getTime(timer);
                time += offset;
                return timer->subscribe(time, id, call_back, timer);
        }
        return -E_NOTFOUND;
}

static inline void cpu_wait_interrupt(int a)
{
        struct sys_cpu* cpu = getcpu(a);
        if (!(hascpu(a) && cpu->halt != NULL ))
                panic("CPU struct not intialised!");

        cpu->halt();
}

static inline int cpu_disable_interrupts(int a)
{
        struct sys_cpu* cpu = getcpu(a);
        if (!(hascpu(a) && cpu->disable_interrupt != NULL ))
                panic("CPU struct not initialised!");

        return cpu->disable_interrupt();
}

static inline int cpu_enable_interrupts(int a)
{
        struct sys_cpu* cpu = getcpu(a);
        if (!(hascpu(a) && cpu->enable_interrupt != NULL ))
                panic("CPU struct not initialised!");
        if ((addr_t) (cpu->enable_interrupt) <= (addr_t) 0xC0000000) {
                panic("Function not in code segment!");
        }

        int ret = cpu->enable_interrupt();
        return ret;
}

static inline void*
get_phys(int cpu, void* virt)
{
        if (!hascpu(cpu) || core.arch->cpu[cpu]->mmu == NULL)
                return NULL ;
        return core.arch->cpu[cpu]->mmu->get_phys(virt);
}

static inline int page_map(int cpu, void* virt, void* phys, int cpl)
{
        if (!hascpu(cpu))
                return -E_NULL_PTR;
        if (core.arch->cpu[cpu]->mmu == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->set_page(virt, phys, cpl);
}

static inline int page_map_range(int cpu, struct sys_mmu_range* range)
{
        if (!hascpu(cpu) || range == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->set_range(range);
}

static inline int page_unmap(int cpu, void* virt)
{
        if (!hascpu(cpu))
                return -E_NULL_PTR;
        if (core.arch->cpu[cpu]->mmu == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->reset_page(virt);
}

static inline int page_unmap_range(int cpu, struct sys_mmu_range* range)
{
        if (!hascpu(cpu) || range == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->reset_range(range);
}

static inline int page_range_cleanup(int cpu, struct sys_mmu_range* range)
{
        if (!hascpu(cpu) || range == NULL)
                return -E_NULL_PTR;
        return core.arch->cpu[cpu]->mmu->cleanup_range(range);
}

int sys_setup_alloc(void);
int sys_setup_devices(void);
int sys_setup_fs(void);
int sys_setup_arch(void);
int sys_setup_modules(void);
int sys_setup_net(void);

void stack_dump(uint32_t* stack_ptr, uint32_t depth);

#ifdef X86
int sys_setup_paging(multiboot_memory_map_t* map, unsigned int length);
#else
int sys_setup_paging(void);
#endif

int interrupt_init();
int32_t interrupt_register(uint16_t interrupt_no,
                int (*procedure)(uint16_t no, uint16_t id, uint64_t r1,
                                uint64_t r2, uint64_t r3, uint64_t r4,
                                void* args), void* args);
int32_t interrupt_deregister(uint16_t interrupt_no, int32_t interrupt_id);
int do_interrupt(uint16_t interrupt_no, uint64_t r1, uint64_t r2, uint64_t r3,
                uint64_t r4);

#ifdef INTERRUPT_TEST
int interrupt_test(int interrupt_no);
#endif

int cpu_timer_init(int cpuid, time_t freq, int16_t irq_no);
int andromeda_timer_init(time_t freq, int16_t irq_no);
struct sys_timer* get_global_timer(int16_t irq_no);
struct sys_timer* get_cpu_timer(int16_t cpu);
#ifdef TIMER_DBG
int timer_setup_test(int16_t cpuid, int16_t irq_no);
#endif

int x86_pit_8253_init(int irq_no, time_t freq);

int hw_interrupt_end(uint16_t irq_no);

#endif
