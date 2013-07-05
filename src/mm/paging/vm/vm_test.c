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

#define SEG_BASE (void*)0xB0000000
#define SEG_SIZE (size_t)0x1000

int vm_test()
{
        struct vm_descriptor* vm1 = vm_new(1);
        struct vm_descriptor* vm2 = vm_new(2);

        if (vm1 == NULL || vm2 == NULL)
        {
                printf("vm test failed on vm descriptor allocation!\n");
                return -E_NULL_PTR;
        }

        struct vm_segment* seg1 = vm_new_segment(SEG_BASE, SEG_SIZE, vm1);
        struct vm_segment* seg2 = vm_new_segment(SEG_BASE, SEG_SIZE, vm2);


        if (seg1 == NULL || seg2 == NULL)
        {
                printf("vm test failed on segment allocation and initialisation!");
                return -E_NULL_PTR;
        }

        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Could not load segment 1!\n");
                return -E_GENERIC;
        }
        memset(SEG_BASE, 'a', SEG_SIZE);
        char* seg_str = SEG_BASE;
        seg_str[SEG_SIZE-1] = 0;

        printf(seg_str);

        if (vm_segment_unload(0, seg1) != -E_SUCCESS)
        {
                printf("Problem unloading segment 1!\n");
                return -E_GENERIC;
        }

        printf("unloaded segment 1\n");
        int i = 0;
        for (; i < 0x3FFFFFFF; i++)
                asm("nop");

        if (vm_segment_load(0, seg2) != -E_SUCCESS)
        {
                printf("Failure loading segment 2!\n");
        }
        memset(SEG_BASE, 'b', SEG_SIZE);
        seg_str[SEG_SIZE-1] = 0;

        printf(seg_str);

        if (vm_segment_unload(0, seg2) != -E_SUCCESS)
        {
                printf("Failure unloading segment 2\n");
                return -E_GENERIC;
        }

        printf("unloaded segment 2\n");
        for (i = 0; i < 0x3FFFFFFF; i++)
                asm("nop");
        if (vm_segment_load(0, seg1) != -E_SUCCESS)
        {
                printf("Issue reloading segment 1\n");
                return -E_GENERIC;
        }
        printf(seg_str);

        printf("\n");
        printf("Seg1: %X\nSeg2: %X\n", (int)seg1, (int)seg2);
        printf("Seg1->range: %X\nSeg2->range: %X\n", (int)(seg1->pages), (int)(seg2->pages));
        printf("seg1->arch: %X\nSeg2->arch: %X\n", (int)(seg1->pages->arch_data), (int)(seg2->pages->arch_data));
        return -E_SUCCESS;
}
