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
        int (*page_alloc)();
        int (*page_free)();
        int (*alloc)();
        int (*free)();
};

struct sys_device_tree {
        int (*suspend)();
        int (*resume)();
        int (*get_dev)();
        int (*set_dev)();
        struct device* root_dev;
};

struct sys_module {
        struct vfile* module;
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
        int (*mount)(struct file*, char* path);
        int (*umount)();
        int (*open)();
        int (*close)();
        int (*read)();
        int (*write)();
        struct tree_root* files;
};

struct system {
        struct arch_abstraction_layer* arch;
        struct sys_memory_manager* mm;
        struct sys_device_tree* devices;
        struct sys_module_tree* kernel_modules;
        struct sys_fs* fs;
        struct sys_net* net;
};

#endif
