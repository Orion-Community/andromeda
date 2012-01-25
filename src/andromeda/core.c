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

#define RL_SHUTDOWN	0x0
#define RL_RUN0		0x1
#define RL_RUN1		0x2
#define RL_RUN2		0x3
#define RL_RUN3		0x4
#define RL_RUN4		0x5
#define RL_REBOOT	0x6

void demand_key();

void buf_dbg()
{
        examine_heap();
        demand_key();
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
        demand_key();
        f->close(f);
        kfree(ret_msg);

        examine_heap();
        demand_key();
        printf(
                "Sizeof file: %X\n"
                "Sizeof buffer: %X\n",
               sizeof(struct vfile),
               sizeof(struct buffer)
        );
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

extern uint32_t key_pressed;

void demand_key()
{
	printf("Press any key to continue!\n");
	key_pressed = 0;
	while(key_pressed == 0)
		halt();
	return;
}

void core_loop()
{
        if (dev_init() != -E_SUCCESS)
                panic("Couldn't initialise /dev");
        init_netif();
        init_network();
        trigger_soft_irq30();
#ifdef SCHED_DBG
        /**
         * Will have to be improved to actually do a context
         * switch here
         */
        pid = syscall(SYS_FORK, 0, 0, 0);
        print_task_stack();
        demand_key();
        syscall(SYS_KILL, pid, 0, 0);
        print_task_stack();
        demand_key();

        struct __THREAD_STATE* t = kalloc(sizeof(struct __THREAD_STATE));
#endif
#ifdef MATH_DBG
        printf("atanh(2.5) = %s\n", (isNaN(atanh(2.5)))?"NaN" : "A number");
#endif
#ifdef BUF_DBG
        buf_dbg();
#endif

        uint32_t pid = 0;

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
//                      sched_next_task();
                        break;

                case RL_REBOOT:
                        syscall(SYS_REBOOT, 0, 0, 0);
                        break;

                case RL_SHUTDOWN:
                        syscall(SYS_SHUTDOWN, 0, 0, 0);
                        break;
                }
                halt(); // Puts the CPU in idle state untill next interrupt
        }
}
