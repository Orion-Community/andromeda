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

#define SEG_BASE_SIMPLE (void*)0xB0000000
#define SEG_SIZE_SMALL (size_t)0x1000

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

        printf(seg_str);

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Problem unloading segment 1!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        printf("unloaded segment 1\n");

        if (vm_segment_load(0, seg2) != -E_SUCCESS)
        {
                printf("Failure loading segment 2!\n");
                ret = -E_GENERIC;
                goto cleanup;
        }
        memset(SEG_BASE_SIMPLE, 'b', 20);
        seg_str[20] = '\n';
        seg_str[21] = 0;

        printf(seg_str);

        if (vm_segment_unload(0, seg2) != -E_SUCCESS)
        {
                printf("Failure unloading segment 2\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        printf("unloaded segment 2\n");
        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Issue reloading segment 1\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        printf(seg_str);

        if (seg_str[0] != 'a')
        {
                printf("Issue switching back to segment 1\n");
                printf("Segment 1 was not correctly protected\n");
                ret = -E_GENERIC;
                goto cleanup;
        }

        printf("Seg1: %X\nSeg2: %X\n", (int)seg1, (int)seg2);
        printf("Seg1->range: %X\nSeg2->range: %X\n", (int)(seg1->pages), (int)(seg2->pages));
        printf("seg1->arch: %X\nSeg2->arch: %X\n", (int)(seg1->pages->arch_data), (int)(seg2->pages->arch_data));

cleanup:
        return ret;
}

int vm_test_large()
{
        int ret = -E_SUCCESS;



        return ret;
}

int vm_test()
{
        int ret = -E_SUCCESS;
        ret = vm_test_small();
        if (ret != -E_SUCCESS)
                return ret;

        ret = vm_test_large();
        if (ret != -E_SUCCESS)
                return ret;

        return -E_SUCCESS;
}
