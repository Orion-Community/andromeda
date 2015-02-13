/**
 *  Andromeda
 *  Copyright (C) 2015  Bart Kuivenhoven
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

#include <types.h>
#include <stdio.h>
#include <andromeda/system.h>
#include <lib/tree.h>

#define FREQUENCY_DIVIDER 1000

static int timer_subscribe_event(time_t time, uint16_t id,
                int (*handler)(uint16_t id, time_t time),
                struct sys_timer* timer);
struct event {
        time_t time;
        int (*handler)(uint16_t id, time_t time);
        uint16_t id;
        struct event* next;
};

static int timer_callback(uint16_t irq_no __attribute__((unused)), uint16_t id,
                uint64_t r1, uint64_t r2, uint64_t r3 __attribute__((unused)),
                uint64_t r4 __attribute__((unused)))
{
        struct sys_timer* timer = NULL;
        /* If this interrupt is global, choose one of the global timers */
        if (r2 != 0x00) {
                /* GLOBAL */
                timer = NULL;
        } else {
                /* */
                warning ("CPUID is static in timer interrupt, change this up please!\n");

                /* Get the appropriate CPU */
                struct sys_cpu* cpu = getcpu(r1); /* This is to work in the future */
                cpu = getcpu(0); /* For now, just assume CPU 0 */
                /* Traverse it's interrupt controller */
                if (cpu->pic == NULL) {
                        panic("No PIC data found for CPU");
                }
                /* And there we have the timer */
                timer = cpu->pic->timers;
        }
        if (timer == NULL) {
                panic("Invalid timer found!");
        }

        /* If the timer callback doesn't match the appropriate ID, panic */
        if (timer->interrupt_id != id) {
                panic("Something happened to our interrupt ID!");
        }

        /* Atomically increment the timer value */
        mutex_lock(&(timer->timer_lock));
        time_t tick = atomic_inc(&(timer->tick));
        if (tick == (uint32_t) (timer->freq) / FREQUENCY_DIVIDER) {
                timer->time++;
                atomic_sub(&(timer->tick), (uint32_t) (timer->freq) / FREQUENCY_DIVIDER);
        }
        mutex_unlock(&(timer->timer_lock));

        /* Now go do something with the available events */
        if (timer->events == NULL) {
                warning("No events could be found due to NULL pointer\n");
        } else {
                /* Find our appropriate event */
                struct event* event = timer->events->find(timer->time,
                                timer->events);
                /* Delete the events */
                timer->events->delete(timer->time, timer->events);

                while (event != NULL ) {
                        if (event->time != timer->time) {
                                timer_subscribe_event(event->time, event->id, event->handler, timer);
                                struct event* last = event;
                                event = event->next;
                                kfree(last);
                                continue;
                        }

                        event->handler(event->id, timer->time);

                        struct event* last = event;
                        event = event->next;
                        kfree(last);
                }
        }

        return -E_SUCCESS;
}

int andromeda_timer_init()
{
        return -E_NOFUNCTION;
}

static int timer_set_freq_dummy(time_t freq, struct sys_timer* timer)
{
        warning("Someone tried to set my (timer)%X to %X\n", (int) timer,
                        (int) freq);
#ifdef DEBUG
        panic("Here you have a nice stack trace!");
#endif
        return -E_NOFUNCTION;
}

static int timer_subscribe_event(time_t time, uint16_t id,
                int (*handler)(uint16_t id, time_t time),
                struct sys_timer* timer)
{
        /* Do some parameter checking */
        if (timer == NULL || handler == NULL) {
                return -E_NULL_PTR;
        }

        /* Allocate a new event */
        struct event* event = kmalloc(sizeof(*event));
        if (event == NULL) {
                return -E_NOMEM;
        }

        /* Set up said event */
        memset(event, 0, sizeof(*event));
        event->handler = handler;
        event->id = id;
        event->time = time;

        /* See if we can find another event at this time */
        volatile struct event* found = timer->events->find((int) time,
                        timer->events);
        /* If not, we'll add this one */
        if (found == NULL) {
                return timer->events->add((int) time, event, timer->events);
        }

        /* If not, we'll add this one to the existing list */
        while (found->next != NULL ) {
                found = found->next;
        }
        found->next = event;

        return -E_SUCCESS;
}

#ifdef TIMER_DBG

static int timer_dbg(uint16_t id, time_t time)
{
        printf("[ DEBUG ] Timer time: %X\n", (int32_t) time);

        if (id != 0) {
                panic("Incorrect ID!");
        }

        struct sys_cpu* cpu = getcpu(0);
        timer_subscribe_event(time + 10, 0, timer_dbg, cpu->pic->timers);

        return 0;
}

#endif

int cpu_timer_init(int cpuid, time_t freq, int irq_no)
{
        int32_t id = interrupt_register(irq_no, timer_callback);
        struct sys_cpu* cpu = getcpu(cpuid);
        if (cpu == NULL) {
                panic("CPU structure not found!");
        }
        if (cpu->pic == NULL) {
                panic("No interrupt controller structure found!");
        }

        struct sys_timer* timer = kmalloc(sizeof(*timer));
        if (timer == NULL) {
                panic("Out of memory!");
        }

        memset(timer, 0, sizeof(*timer));

        timer->events = tree_new_avl();
        timer->freq = freq;
        timer->time = 0;
        timer->set_freq = timer_set_freq_dummy;
        timer->subscribe = timer_subscribe_event;
        timer->interrupt_id = id;

        cpu->pic->timers = timer;

#ifdef TIMER_DBG
        timer_subscribe_event(10, 0, timer_dbg, timer);
#endif

        return -E_NOFUNCTION;
}

