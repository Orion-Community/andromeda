/*
 * Andromeda
 * Copyright (C) 2012 - 2013  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <andromeda/system.h>
#include <mm/vm.h>
#include <andromeda/core.h>

#define SEG_BASE_SIMPLE (void*)0xB0000000
#define SEG_BASE_ONE SEG_BASE_SIMPLE
#define SEG_BASE_TWO SEG_BASE_SIMPLE+0x4000
#define SEG_BASE_THR SEG_BASE_SIMPLE+0x8000

#define SEG_SIZE_SMALL (size_t)0x1000
#define SEG_SIZE_LARGE (size_t)(1 << 22)

int vm_test_small()
{
        int ret = -E_SUCCESS;
        struct vm_descriptor* vm1 = vm_new(1);
        struct vm_descriptor* vm2 = vm_new(2);

        if (vm1 == NULL || vm2 == NULL)
        {
                printf("vm test failed on vm descriptor allocation!\n");
                return -E_NULL_PTR;
        }

        struct vm_segment* seg1 = vm_new_segment(SEG_BASE_SIMPLE, SEG_SIZE_SMALL, vm1);
        struct vm_segment* seg2 = vm_new_segment(SEG_BASE_SIMPLE, SEG_SIZE_SMALL, vm2);

        if (seg1 == NULL || seg2 == NULL)
        {
                printf("vm test failed on segment allocation and initialisation!");
                vm_free(vm1);
                vm_free(vm2);
                return -E_NULL_PTR;
        }

        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Could not load segment 1!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }
        memset(SEG_BASE_SIMPLE, 'a', 20);
        char* seg_str = SEG_BASE_SIMPLE;
        seg_str[20] = '\n';
        seg_str[21] = 0;

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Problem unloading segment 1!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        if (vm_segment_load(0, seg2) != -E_SUCCESS)
        {
                printf("Failure loading segment 2!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }
        memset(SEG_BASE_SIMPLE, 'b', 20);
        seg_str[20] = '\n';
        seg_str[21] = 0;

        if (vm_segment_unload(0, seg2) != -E_SUCCESS)
        {
                printf("Failure unloading segment 2\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Issue reloading segment 1\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        if (seg_str[0] != 'a')
        {
                printf("Issue switching back to segment 1\n");
                printf("Segment 1 was not correctly protected\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Failure unloading segment 1\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

cleanup:
        vm_free(vm1);
        vm_free(vm2);
        return ret;
}

int vm_dump_ranges(struct vm_range_descriptor* r)
{
        if (r == NULL)
                return -E_NULL_PTR;

        printf("this: %X\tnext: %X\tprev: %X\n", r, r->next, r->prev);
        printf("base: %X\n", r->base);
        printf("size: %X\n", r->size);

        vm_dump_ranges(r->next);
        return -E_SUCCESS;
}

int vm_dump_segments(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        printf("Segment: %s\n", s->name);
        printf("virt: %X\n", s->virt_base);
        printf("size: %X\n", s->size);

        printf("\nFree:\n");

        mutex_lock(&s->lock);
        struct vm_range_descriptor* free = s->free;

        vm_dump_ranges(free);
        printf("Allocated:\n");
        mutex_unlock(&s->lock);

        mutex_lock(&s->lock);
        struct vm_range_descriptor* allocated = s->allocated;
        vm_dump_ranges(allocated);

        struct vm_segment* sn = s->next;
        mutex_unlock(&s->lock);

        printf("next segment: %X\n", sn);

        vm_dump_segments(sn);
        return -E_SUCCESS;
}

int vm_dump(struct vm_descriptor* v)
{
        if (v == NULL)
                return -E_NULL_PTR;

        printf("cpl: %X\npid: %X\n", v->cpl, v->pid);
        printf("Segments: \n");
        mutex_lock(&v->lock);
        vm_dump_segments(v->segments);
        mutex_unlock(&v->lock);

        return -E_SUCCESS;
}

int vm_test_alloc()
{

        struct vm_segment* heap = vm_find_segment(".heap");
        if (heap == NULL)
                return -E_NULL_PTR;


        if (heap->free == NULL)
                return -E_HEAP_GENERIC;
        size_t free_state = heap->free->size;

        void* tst = vm_get_kernel_heap_pages(0x1000);
        void* tst2 = vm_get_kernel_heap_pages(0xb1aa7);

        if (heap->free == NULL)
                return -E_HEAP_GENERIC;

        size_t predicted = free_state - 0x1000;
        predicted -= (predicted % 0x4000);
        predicted -= 0xb1aa7;
        predicted -= (predicted % 0x4000);
        if (heap->free->size != predicted)
        {
                printf("Something went wrong in allocation!\n");
                printf("Heap free: %X\n", (int)heap->free->size);
                printf("Pred free: %X\n", (int)predicted);
                return -E_HEAP_GENERIC;
        }

        vm_free_kernel_heap_pages(tst2);

        predicted += 0xb1aa7;
        predicted += (0x4000 - (predicted % 0x4000));

        if (heap->free->size != predicted)
        {
                printf("Something went wrong in allocation!\n");
                printf("Heap free: %X\n", (int)heap->free->size);
                printf("Pred free: %X\n", (int)predicted);
                return -E_HEAP_GENERIC;
        }

        vm_free_kernel_heap_pages(tst);

        if (heap->free->size != free_state)
        {
                printf("End state does not match start state, something is wrong!\n");
                return -E_HEAP_GENERIC;
        }

        return -E_SUCCESS;
}

int vm_test_large()
{
        int ret = -E_SUCCESS;

        struct vm_descriptor* vm1 = vm_new(0);
        struct vm_descriptor* vm2 = vm_new(1);

        if (vm1 == NULL || vm2 == NULL)
                return -E_NULL_PTR;

        struct vm_segment* seg1 = vm_new_segment(SEG_BASE_SIMPLE, SEG_SIZE_LARGE, vm1);
        struct vm_segment* seg2 = vm_new_segment(SEG_BASE_SIMPLE, SEG_SIZE_LARGE, vm2);

        if (vm1->segments == NULL || vm2->segments == NULL)
        {
                ret = -E_NULL_PTR;
                goto cleanup;
        }

        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Loading of big segment 1 failed\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        memset(SEG_BASE_SIMPLE, 'c', SEG_SIZE_LARGE);

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Issue in unloading big segment 1\n");
                ret = -E_GENERIC;
                goto cleanup;
        }
        if (vm_segment_load(0, seg2) != -E_SUCCESS)
        {
                printf("Problem loading big segment 2\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        memset(SEG_BASE_SIMPLE, 'd', SEG_SIZE_LARGE);

        if (vm_segment_unload(0, seg2) != -E_SUCCESS)
        {
                printf("Problem unloading big segment 2\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Trouble when loading big segment 1 for the 2nd time\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        char* seg_str = SEG_BASE_SIMPLE;
        int i = 0;
        for (; i < SEG_SIZE_LARGE; i+= 0x1000)
        {
                if (seg_str[i] != 'c')
                {
                        printf("Uh-oh, our big segment got damaged!");
                        ret = -E_GENERIC;
                        goto cleanup;
                }
        }

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Barney rouble when unloading big segment 1 for the 2nd time!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        vm_segment_unload(0, seg1);

cleanup:
        vm_free(vm1);
        vm_free(vm2);
        return ret;
}

int vm_test()
{
        int ret = -E_SUCCESS;

        ret = vm_test_alloc();
        if (ret != -E_SUCCESS)
                return ret;

        ret = vm_test_small();
        if (ret != -E_SUCCESS)
                return ret;

        ret = vm_test_large();
        if (ret != -E_SUCCESS)
                return ret;

        return -E_SUCCESS;
}
