/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
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
/**
 * \AddToGroup paging
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <boot/mboot.h>
#include <andromeda/system.h>

/**
 * \fn mboot_map_special_entry
 * \brief Set a specific entry in the page list to a certain value
 * \param ptr
 * \brief Where does the physical page reside
 * \param virt
 * \brief Where does the virtual page reside
 * \param size
 * \brief what's the amount of pages that need to be marked (in bytes)
 * \param free
 * \brief Can the descriptor be used for other things than kernel purposes?
 * \param dma
 * \brief Is hardware mapped to this page or not
 * \return A generic error code
 */
int
mboot_map_special_entry(addr_t ptr,addr_t virt,size_t size,bool free,bool dma)
{
        struct mm_page_descriptor* tmp;
        tmp = kmalloc(sizeof(*tmp));

        if (tmp == NULL)
                panic("OUT OF MEMORY!");

        memset(tmp, 0, sizeof(*tmp));
        tmp->page_ptr = (void*)ptr;
        tmp->virt_ptr = (void*)virt;
        tmp->size = size;
        tmp->free = free;
        tmp->dma = dma;
        if (free)
                mm_page_append(&free_pages, tmp);
        else
                mm_page_append(&allocated_pages, tmp);

        return -E_SUCCESS;
}

#ifdef MODULES
/**
 * \fn mboot_map_module
 * \brief Use this function to map kernel modules into the page lists
 * \param module
 * \brief The module to map
 */
int
mboot_map_module(struct multiboot_mod_list* module)
{
        addr_t module_start = module->mod_start;
        addr_t module_end = module->mod_end;

        printf("mod_start: %X\tmod_end: %X\n", module_start, module_end);
        if (module_start > module_end)
        {
                module_start ^= module_end;
                module_end ^= module_start;
                module_start ^= module_end;
                printf("mod_start: %X\tmod_end: %X\n", module_start, module_end);
        }
        printf("mod_size: %X\n", module_end - module_start);
        char* data = (char*)(module_start+0xC0000000-(module_end-module_start-16));
        idx_t i = 0;
        for (; i < module_end - module_start; i++)
                putc(*(data++));

        return -E_NOFUNCTION;
}
#endif

int
mboot_map_modules(struct multiboot_mod_list *modules, idx_t no_mods)
{
#ifdef MODULES
        register idx_t i = 0;
        printf("No modules: %X\n", no_mods);
        printf("End ptr: %X\n", ((int)(&end)));
        for (; i < no_mods; i++)
        {
                int ret = mboot_map_module(&modules[i]);
                if (ret != -E_SUCCESS)
                        return ret;
        }
#endif
        return -E_SUCCESS;
}

/**
 * \fn mboot_page_setup
 * \brief Setup the multiboot memory region list
 * \param map
 * \brief The pointer to the list of regions
 * \param mboot_map_size
 * \brief How big is the list?
 * \return A generic error code
 */
int
mboot_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        multiboot_memory_map_t* mmap = map;

        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {
                #ifdef PAGE_ALLOC_DBG
                debug("Size: %X\tbase: %X\tlength: %X\ttype: %X\n",
                      mmap->size,
                      (uint32_t)mmap->addr,
                      (uint32_t)mmap->len,
                      mmap->type
                );
                #endif

                struct mm_page_descriptor* tmp;
                if (mmap->addr < SIZE_MEG)
                {
                        if (mmap->addr+mmap->size > SIZE_MEG)
                        {
                                mboot_map_special_entry(SIZE_MEG, SIZE_MEG,
                                                 mmap->addr+mmap->size-SIZE_MEG,
                                       (mmap->type == 1) ? TRUE : FALSE, FALSE);
                        }
                        goto itteration_skip;
                }
                tmp = kmalloc(sizeof(*tmp));
                if (tmp == NULL)
                        panic("Out of memory!");
                memset(tmp, 0, sizeof(*tmp));

                tmp->page_ptr = (void*)((addr_t)mmap->addr);
                tmp->virt_ptr = tmp->page_ptr;
                tmp->size = (size_t)mmap->len;

                if (mmap->type != 1)
                {
                        if (mm_page_append(&allocated_pages, tmp) == NULL)
                                panic("Couldn't add page!");
                        tmp->free = FALSE;
                }
                else
                {
                        if (mm_page_append(&free_pages, tmp) == NULL)
                                panic("Couldn't add page!");
                        tmp->free = TRUE;
                }
                itteration_skip:
                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }
#ifdef PAGE_ALLOC_DBG
        debug("\nFirst run\n");
        mm_show_pages();
#endif
        x86_page_map_higher_half();
#ifdef PAGE_ALLOC_DBG
        debug("\nSecond run\n");
        mm_show_pages();
#endif
        x86_map_kernel();
#ifdef PAGE_ALLOC_DBG
        debug("\nThird run (maps the kernel)\n");
        mm_show_pages();

        /** mm_page_alloc doesn't move the page to the allocated list */
        void* addr = mm_page_alloc(0x1000);
        debug("\nFourth run\n");
        mm_show_pages();

        mm_page_free(addr);
        debug("\nFifth run\n");
        mm_show_pages();
#endif
        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
