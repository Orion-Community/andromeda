/*
    Andromeda
    Copyright (C) 2011  Bart Kuivenhoven

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

#include <stdlib.h>
#include <andromeda/core.h>
#include <andromeda/sched.h>
#include <fs/path.h>
#include <andromeda/syscall.h>
#include <andromeda/drivers.h>
#include <networking/rtl8168.h>
#include <networking/net.h>
#include <andromeda/buffer.h>
#include <mm/cache.h>
#include <mm/vm.h>
#include <stdio.h>

#define RL_SHUTDOWN     0x0
#define RL_RUN0         0x1
#define RL_RUN1         0x2
#define RL_RUN2         0x3
#define RL_RUN3         0x4
#define RL_RUN4         0x5
#define RL_REBOOT       0x6

void demand_key();

void buf_dbg()
{
        struct vfile* f = kalloc(sizeof(struct vfile));
        if (f == NULL)
                panic("No mem in buf_dbg");

        memset(f, 0, sizeof(struct vfile));

        int ret = buffer_init(f, 0x8, 0);
        if (ret != -E_SUCCESS)
        {
                debug("Return value from init: %X\n", ret);
                panic("Buffer initialisation not successful!");
        }

        char *blaat = "Schaap\n";
        char *ret_msg = kalloc(sizeof("Schaap\n"));
        memset(ret_msg, 0, sizeof(*blaat));

        f->write(f, blaat, strlen(blaat));
        f->seek(f, 0, SEEK_SET);
        f->read(f, ret_msg, strlen(blaat));

        printf("MSG: %s\n", ret_msg);

        memset(ret_msg, 0, sizeof(*blaat));
        f->seek(f, 0x1000-3, SEEK_SET);
        f->write(f, blaat, strlen(blaat));
        f->seek(f, -((int64_t)strlen(blaat)), SEEK_CUR);
        f->read(f, ret_msg, strlen(blaat));

        printf("MSG: %s\n", ret_msg);
        f->close(f);
        kfree(ret_msg);
}

void shutdown()
{
        printf("You can now shutdown your PC\n");
        for(;;)
        {
                endProg();
        }
}

volatile uint32_t rl = RL_RUN0;

void init_set(uint32_t i)
{
        debug("Changing run level to %i\n", i);
        rl = i;
}

void test_sprintf()
{
        char *test = kalloc(255);
        if (test == NULL)
                panic("OUT OF MEMORY!");
        memset(test, 0, 255);
        int ret = sprintf(test, "hello: %8X\n", 0xC0DE);
        printf("%X\t%s", ret, test);

        free(test);
}

extern int page_table_boot;

void core_loop()
{
//         init_netif();
//         init_network();

#ifdef DBG
#ifdef MATH_DBG
        printf("atanh(2.5) = %s\n", (isNaN(atanh(2.5)))?"NaN" : "A number");
#endif
#ifdef BUF_DBG
        buf_dbg();
#endif
#ifdef VMEM_DBG
        debug("Address of higher half: %X\n", (int)&higherhalf);
        debug("Address of end ptr:     %X\n", (int)&end);
#endif
#ifdef SPRINTF_DBG
        test_sprintf();
        test_calculation_functions();
#endif
#ifdef SLAB_DBG
        mm_cache_test();
#endif
#endif
#ifdef VM_DBG
        vm_test();
#endif
#ifdef RR_EXP
        extern void task_testA();
        extern void task_testB();
        rr_thread_init(task_testA);
        rr_thread_init(task_testB);
#endif
#ifdef PT_DBG
        addr_t ptb = (addr_t)(&page_table_boot) + 0xC0000000;
        printf( "page table boot: %X\n"
                "phys: %X\n"
                "virt: %X\n",
                &core_loop,
                x86_pte_get_phys(core_loop),
                core_loop
        );
#endif

//         uint32_t pid = 0;
        debug("Entering core loop\n");
        while (TRUE) // Infinite loop, to make the kernel wait when there is nothing to do
        {
                switch (rl)
                {
                case RL_RUN0:
                case RL_RUN1:
                case RL_RUN2:
                case RL_RUN3:
                case RL_RUN4:
                     halt();
#ifdef RR_EXP
                     rr_sched();
#endif
                        break;

                case RL_REBOOT:
                        break;

                case RL_SHUTDOWN:
                        break;
                }
                halt(); // Puts the CPU in idle state untill next interrupt
        }
}
