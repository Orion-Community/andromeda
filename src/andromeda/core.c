/*
    Andromeda
    Copyright (C)  2011, 2012, 2013, 2014, 2015  Bart Kuivenhoven
    Copyright (C)  2011  Steven van der Schoot
    Copyright (C)  2012  Michel Megens

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESbuffer_initS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <andromeda/core.h>
#include <andromeda/sched.h>
#include <andromeda/system.h>
#include <fs/path.h>
#include <andromeda/syscall.h>
#include <andromeda/drivers.h>
#include <networking/rtl8168.h>
#include <networking/net.h>
#include <lib/tree.h>
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

/**
 * \var stack
 * \brief Kernel stack
 */
unsigned char stack[STD_STACK_SIZE];

struct task *current_task = NULL;

void demand_key();

//struct task *current_task = NULL;

void shutdown()
{
        printf("You can now shutdown your PC\n");
        for(;;)
        {
                endProg();
        }
}

#ifdef TREE_TEST
/**
 * \fn tree_cleanup_test
 * \brief Tree object destructor
 * \param a
 * \brief Pointer to the object to be removed
 * \param b
 * \brief unused
 *
 * When the tree has to remove a node, because it is flushed or otherwise
 * instructed to remove the node, it has the option of either calling the
 * destructor, if present or leaving the clean-up to be done by other code.
 *
 * The option to clean up after your self rather than in the tree can be useful
 * if the objects are also referenced elsewhere.
 *
 * Alternatively, these references can be counted in the object itself and with
 * that information be assured of a proper clean-up when necessary.
 */
static int tree_cleanup_test(void* a, void* b __attribute__((unused)))
{
        kfree((int*)a);
        return -E_SUCCESS;
}

/**
 * \fn tree_test
 * \brief Test the tree structure in a really basic manner
 *
 * This function tries to create a tree, fills it with mock data before trying
 * to remove it all again.
 *
 * This test should be used in tandem with the GDB connection on QEMU.
 */
void tree_test()
{
        struct tree_root* root = tree_new_avl();
        int i = 0;
        for (; i < 10; i++)
        {
                void* tst = kmalloc(sizeof(int));
                root->add(i, tst, root);
        }

        root->purge(root, tree_cleanup_test, NULL);
}
#endif

volatile uint32_t rl = RL_RUN0;

void init_set(uint32_t i)
{
        debug("Changing run level to %i\n", i);
        rl = i;
}

void test_sprintf()
{
        char *test = kmalloc(255);
        if (test == NULL)
                panic("OUT OF MEMORY!");
        memset(test, 0, 255);
        int ret = sprintf(test, "hello: %8X\n", 0xC0DE);
        printf("%X\t%s", ret, test);

        kfree(test);
}

#ifdef PT_DBG
extern int page_table_boot;
#endif

void core_loop()
{
//         init_netif();
//         init_network();

#ifdef TREE_TEST
        tree_test();
#endif
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

#ifdef SC_TEST
        // Let's go about testing the system calls
        char* str = "Hello world!\n";
        int i = sc_call(SYS_WRITE, 0, 0, (int)str, strlen(str));
        printf ("sc: %X\n", -i);
#endif
#ifdef INTERRUPT_TEST
        interrupt_test(80);
#endif
        debug ("Entering core loop\n");
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
                halt(); // Puts the CPU in idle state until next interrupt
        }
}
