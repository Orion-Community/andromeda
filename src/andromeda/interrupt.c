/*
 *   Copyright (C) 2015  Bart Kuivenhoven
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <andromeda/system.h>
#include <arch/x86/cpu.h>

#ifdef SLAB
static struct mm_cache* interrupt_cache;
#endif

static int interrupt_initialised = 0;

struct interrupt {
        int (*procedure)(uint16_t interrupt_no, uint16_t interrupt_id,
                        uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4,
                        void* args);
        uint16_t id;
        void* args;

        struct interrupt* next;
};

static mutex_t interrupt_lock = mutex_unlocked;

#define INTERRUPTS 255

static struct interrupt interrupts[INTERRUPTS];

#ifdef SLAB
static void interrupt_dtor(void* object,
                struct mm_cache* cache __attribute__((unused)),
                uint32_t flags __attribute__((unused)))
{
        struct interrupt* i = (struct interrupt*) object;

        memset(i, 0, sizeof(*i));
}
#endif

int interrupt_init()
{
        if (interrupt_initialised != 0) {
                return -E_ALREADY_INITIALISED;
        }
        memset(interrupts, 0, sizeof(interrupts));

#ifdef SLAB
        interrupt_cache = mm_cache_init("slab", sizeof(struct interrupt), 0,
        NULL, interrupt_dtor);
#endif

        interrupt_initialised = -1;
        return -E_SUCCESS;
}

int32_t interrupt_register(uint16_t interrupt_no,
                int (*procedure)(uint16_t no, uint16_t id, uint64_t r1,
                                uint64_t r2, uint64_t r3, uint64_t r4,
                                void* args), void* args)
{
        if (interrupt_initialised == 0) {
                panic("To early with registering interrupts!");
        }
        if (interrupt_no >= INTERRUPTS) {
                return -E_OUTOFBOUNDS;
        }

        if (procedure == NULL) {
                return -E_INVALID_ARG;
        }

        int32_t id = 0;
        volatile int int_state = cpu_disable_interrupts(0);
        mutex_lock(&interrupt_lock);
        /* If the root node is empty, use that */
        if (interrupts[interrupt_no].procedure == NULL) {
                interrupts[interrupt_no].procedure = procedure;
                interrupts[interrupt_no].args = args;
                id = interrupts[interrupt_no].id;
                goto unlock;
        }

        /* Root node wasn't empty, let's add a new one at the end of the list */
        struct interrupt* i = &interrupts[interrupt_no];
        struct interrupt* prev = i;
        while (i != NULL && i->procedure != NULL ) {
                if (id < i->id) {
                        id = i->id;
                }
                prev = i;
                i = i->next;
        }

        if (id == (1 << 16) - 1) {
                id = -E_OUTOFBOUNDS;
                goto unlock;
        }

#ifdef SLAB
        prev->next = mm_cache_alloc(interrupt_cache, 0);
#else
        prev->next = kmalloc(sizeof(*i));
#endif
        i = prev->next;
        memset(i, 0, sizeof(*i));
        i->id = ++id;
        i->procedure = procedure;
        i->args = args;

        unlock: mutex_unlock(&interrupt_lock);
        if (int_state == INTERRUPTS_ENABLED) {
                cpu_enable_interrupts(0);
                //hw_interrupt_end(interrupt_no);
        }

        return (int32_t) id;
}

int32_t interrupt_deregister(uint16_t interrupt_no, int32_t interrupt_id)
{
        if (interrupt_no >= INTERRUPTS || interrupt_id >= ((1 << 16) - 1))
                return -E_OUTOFBOUNDS;

        volatile int int_state = disableInterrupts();
        mutex_lock(&interrupt_lock);
        int32_t ret = -E_SUCCESS;

        struct interrupt* i = &interrupts[interrupt_no];

        if (interrupts[interrupt_no].id == interrupt_id) {
                struct interrupt* next = i->next;
                if (next == NULL) {
                        memset(i, 0, sizeof(*i));
                } else {
                        memcpy(i, next, sizeof(*i));
#ifdef SLAB
                        mm_cache_free(interrupt_cache, next);
#else
                        kfree(next);
#endif
                }

                goto unlock;
        }

        struct interrupt* prev = NULL;
        for (; i != NULL ; prev = i, i = i->next) {
                if (i->id == interrupt_id) {
                        prev->next = i->next;
#ifdef SLAB
                        mm_cache_free(interrupt_cache, i);
#else
                        kfree(i);
#endif
                        goto unlock;
                }
        }

        unlock: mutex_unlock(&interrupt_lock);
        if (int_state == INTERRUPTS_ENABLED) {
                enableInterrupts();
        }
        return ret;
}

int do_interrupt(uint16_t interrupt_no, uint64_t r1, uint64_t r2, uint64_t r3,
                uint64_t r4)
{
        int ret = -E_SUCCESS;

        if (interrupt_no >= INTERRUPTS) {
                return -E_OUTOFBOUNDS;
        }

        int interrupt_state = cpu_disable_interrupts(0);

        struct interrupt* i = &interrupts[interrupt_no];
        for (; i != NULL && i->procedure != NULL ; i = i->next) {
                ret |= i->procedure(interrupt_no, i->id, r1, r2, r3, r4,
                                i->args);

                if (ret != -E_SUCCESS) {
                        printf("Failure in interrupt %X\n", interrupt_no);
                        printf("Specific procedure at %X\n",
                                        (int) (i->procedure));

                        panic("Go kick the interrupt author, "
                                        "this shouldn't happen!");
                }
        }
        if (interrupt_state != 0) {
                cpu_enable_interrupts(0);
        }

        return -E_SUCCESS;
}

#ifdef INTERRUPT_TEST

static uint16_t test_interrupt_no;
static uint16_t test_interrupt_id;
static uint16_t test_interrupt_success;

static int interrupt_test_func(uint16_t interrupt_no, uint16_t interrupt_id,
                uint64_t r1, uint64_t r2, uint64_t r3, uint64_t r4, void* args)
{
        test_interrupt_success = 0;
        if (interrupt_id != test_interrupt_id) {
                test_interrupt_success = -2;
        }
        if (interrupt_no != test_interrupt_no) {
                test_interrupt_success = -1;
        }
        if (r1 != 1 || r2 != 2 || r3 != 3 || r4 != 4) {
                test_interrupt_success = -3;
        }

        if (args != NULL) {
                test_interrupt_success = -4;
        }
        return -E_SUCCESS;
}

int interrupt_test(int interrupt_no)
{
        interrupt_init();
        if (interrupt_no > (1 << 15)) {
                return -E_OUTOFBOUNDS;
        }

        test_interrupt_no = (uint16_t) interrupt_no;
        test_interrupt_id = interrupt_register(test_interrupt_no,
                        interrupt_test_func, NULL);

        do_interrupt(test_interrupt_no, (uint64_t) 1, (uint64_t) 2,
                        (uint64_t) 3, (uint64_t) 4);

        if (test_interrupt_success) {
                printf("Interrupt test failed! %X\n", test_interrupt_success);
        }

        interrupt_deregister(test_interrupt_no, test_interrupt_id);

        return -E_SUCCESS;
}

#endif
