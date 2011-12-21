/*
 Orion OS, The educational operatingsystem
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

#include <mm/paging.h>
#include <mm/map.h>
#include <stdlib.h>
#include <mm/heap.h>
#include <Andromeda/error.h>
#include <arch/intel/task.h>

#define PRESENTBIT    0x01
#define WRITEBIT      0x02
#define USERBIT       0x04
#define RESERVEDBIT   0x08
#define DATABIT       0x10

#define PRESENT(a)  ((a & PRESENTBIT)  ? TRUE : FALSE)
#define WRITE(a)    ((a & WRITEBIT)    ? TRUE : FALSE)
#define USER(a)     ((a & USERBIT)     ? TRUE : FALSE)
#define RESERVED(a) ((a & RESERVEDBIT) ? TRUE : FALSE)
#define DATA(a)     ((a & DATABIT)     ? FALSE : TRUE)

volatile mutex_t page_lock = 0;
boolean pageDbg = false;

volatile addr_t offset = 0xC0000000;

addr_t idx_memory_mapped_io = MAP_NOMAP;
addr_t idx_kernel_space = MAP_NOMAP;

addr_t virt_page_dir[PAGETABLES];

int page_alloc_page(uint32_t, addr_t, struct page_dir*, boolean);

/**
 * The andromeda paging system is set up here.
 * 
 * This does come with the warning that this isn't finished by a very very very
 * long shot. Feel free to contribute.
 */

addr_t getPageDir()
{
#ifdef PAGEDBG
  printf("WARNING! Page directory address not reliable!\n");
#endif
  return getCR3() + offset;
}

/**
 * Called when a pagefault occurs, is in charge of fixing the fault and swapping
 * if necessary.
 */
void cPageFault(isrVal_t registers)
{
	addr_t page = getCR2();
	addr_t page_addr = page & ~(0xFFF);

#ifdef PAGEDBG
	printf("PG!\n");
	printf("Fault addr: %X\nPage index: %X\n", page, page_addr);
	printf("Fault type: %X\n", registers.errCode);
	printf("EIP: %X\nESP: %X\nESP: %X\n",
			       registers.eip, registers.procesp, registers.esp);
	printf("eax: %X\tebx: %X\necx: %X\tedx: %X\n",
		    registers.eax, registers.ebx, registers.ecx, registers.edx);
#endif

	if (registers.cs != 0x8 && registers.cs != 0x18)
		panic("Incorrect frame!");
	if (USER(registers.errCode))
		panic("Userspace isn't implemented yet!");
	if (RESERVED(registers.errCode))
		panic("A reserved bit has been set!\n");
	if (PRESENT(registers.errCode))
		panic("Illegal operation!");

	addr_t pd = getPageDir();

	/**
	* The data bit only works if a specific bit is set. See intel docs volume 3
	* for more information.
	*/

	if (DATA(registers.errCode))
	{
#ifdef PAGEDBG
		printf("Trying to access unimplemented data!\n");
#endif
		if (WRITE(registers.errCode))
		{
#ifdef PAGEDBG
		printf("Faulted a write attempt!\n");
		printf("Adding page!\n");
#endif
		if (USER(registers.errCode))
		{
			//Add a user page!
		}
		else
		{
#ifdef PAGEDBG
			printf("Adding a kernel page!\n");
#endif
			if (idx_kernel_space == MAP_NOMAP)
				panic("Kernel page map not correctly initialised!");

			int ret = page_alloc_page
				(idx_kernel_space, page_addr, (void*)pd, FALSE);
			if (ret != -E_SUCCESS)
			{
				printf("ERRCODE: %X\n", -ret);
				panic("Couldn't alloc page!");
			}
#ifdef PAGEDBG
			printf("Phys of %X = %X\n", page, page_phys_addr(page, (void*)pd));
#endif
			}
		}
		else
		{
#ifdef PAGEDBG
			printf("Faulted a read attempt!\n");
#endif
			// Assume the page may be read!
			if (idx_kernel_space == MAP_NOMAP)
			    panic("Kernel page map not correctly initialised!");
			int ret = page_alloc_page(idx_kernel_space, page_addr,
							      (void*)pd, FALSE);
			if (ret != -E_SUCCESS)
			{
				printf("ERRCODE: %X\n", -ret);
				panic("Couldn't alloc page!");
			}
#ifdef PAGEDBG
			printf("Phys of %X = %X\n", page, page_phys_addr(page, (void*)pd));
#endif
		}
	}
	else
	{
#ifdef PAGEDBG
	panic("Trying to run unimplemented code!\n");
#endif
	}
#ifdef UNDEFINED
	printf("Page faults currently under construction!\n");
#endif
}

/**
 * A bitmap of the ammount of pages in use in every pagetable
 */
uint16_t page_cnt[PAGETABLES];
#ifdef PAGEDBG
uint32_t pt_allocs = 0;
uint32_t pt_uses = 0;
#endif

void
page_map_kernel_entry(addr_t virtual, addr_t phys)
{
#ifdef PAGEDBG
        printf("Mapping virt addr %x to %x.\n",virtual,phys);
#endif
        page_map_entry(virtual&(~0xfff), phys&(~0xfff), (void*)getPageDir(),
                                                                         FALSE);
}

/**
 * Used to map a page to a physical addess, with the option to put it in
 * usermode and to choose a different page directory than the current (usefull
 * when using multiple processors).
 */
int page_map_entry(addr_t virtual, addr_t physical, struct page_dir *pd,
                                                               boolean userMode)
{
        if ((virtual % 0x1000) || (physical % 0x1000))
                panic("AIEEE!!! Virtual or physical address not alligned!!!");
        while(mutexTest(page_lock))
        {
#ifdef PAGEDBG
                printf("Paging is locked!\n");
#endif
        }
        struct page_table* pt;
        addr_t pd_entry = virtual >> 22 % 0x400;
        addr_t pt_entry = (virtual >> 12) % 0x400;

        if (pd[pd_entry].present == FALSE)
        {
#ifdef PAGEDBG
                pt_allocs++;
#endif
                pt = alloc(sizeof(pt)*PAGETABLES, TRUE);
                virt_page_dir[pd_entry] = (addr_t)pt;
                if (pt == NULL)
                {
                        mutexRelease(page_lock);
                        ol_dbg_heap();
                        panic("NOMEM!");
                        return -E_NOMEM;
                }
                memset(pt, 0, sizeof(pt)*PAGETABLES);
                pd[pd_entry].pageIdx  = ((addr_t)(pt)-offset) >> 12;
                pd[pd_entry].present  = 1;
                pd[pd_entry].rw       = 1;
                pd[pd_entry].userMode = 1;
                pd[pd_entry].pwt      = 0;
                pd[pd_entry].pcd      = 0;
                pd[pd_entry].accessed = 0;
                pd[pd_entry].dirty    = 0;
                pd[pd_entry].pageSize = 0;
                pd[pd_entry].global   = 0;
                pd[pd_entry].ignored  = 0;
        }
	else
	{
#ifdef PAGEDBG
		pt_uses++;
#endif
		pd[pd_entry].userMode = TRUE;
		pt = (void*)virt_page_dir[pd_entry];
		if (pt == NULL)
		{
			mutexRelease(page_lock);
			printf("nomem\n");
			ol_dbg_heap();
			panic("NOMEM");
			return -E_PAGE_MAPPING;
		}
	}
	if (pt[pt_entry].present == TRUE)
	{
		mutexRelease(page_lock);
#ifdef PAGEDBG
		printf("WARNING! Page already mapped! Unmap it first\n");
#endif
		return -E_PAGE_MAPPING;
	}

	pt[pt_entry].pageIdx  = physical >> 12;
	pt[pt_entry].present  = 1;
	pt[pt_entry].rw       = 1;
	pt[pt_entry].userMode = userMode;
	pt[pt_entry].pwt      = 0;
	pt[pt_entry].pcd      = 0;
	pt[pt_entry].dirty    = 0;
	pt[pt_entry].accessed = 0;
	pt[pt_entry].pat      = 0;
	pt[pt_entry].global   = 0;
	pt[pt_entry].ignored  = 0;

	page_cnt[pd_entry]++;
	mutexRelease(page_lock);
#ifdef PAGEDBG
	printf("Virtual: %X\tPhys: %X\tidx: %X\n", virtual, physical,
                                                          pt[pt_entry].pageIdx);
#endif
	return -E_SUCCESS;
}

/**
 * Function does exactly what it says on the tin (or header for that matter)
 */
int page_release_entry(addr_t virtual, struct page_dir *pd)
{
	while (mutexTest(page_lock))
	{
#ifdef PAGEDBG
		printf("Paging is locked!\n");
#endif
	}
	addr_t pd_entry = virtual >> 22;
	addr_t pt_entry = (virtual >> 12) % 0x400;

	if (pd[pd_entry].present == FALSE)
	{
		mutexRelease(page_lock);
		return -E_PAGE_NOPAGE;
	}

	struct page_table *pt = (struct page_table*)
                                        ((pd[pd_entry].pageIdx) >> 12) + offset;
	if (pt[pt_entry].present == FALSE)
	{
		mutexRelease(page_lock);
		return -E_PAGE_NOPAGE;
	}

	pt[pt_entry].present = FALSE;
	pt[pt_entry].pageIdx = 0;

	page_cnt[pd_entry]--;
	if (page_cnt[pd_entry] == 0)
	{
		pd[pd_entry].present = FALSE;
		free((void*)((pd[pd_entry].pageIdx+offset)*PAGESIZE));
		pd[pd_entry].pageIdx = 0;
		virt_page_dir[pd_entry] = 0;
	}
	mutexRelease(page_lock);
	return 0;
}

/**
 * This is ought to set up the basic paging system, for the code to be remapped
 * to higher half.
 */
addr_t setup_page_dir()
{
	/**
	* Make the page directory
	*/
	struct page_dir *pd = alloc(sizeof(pd) * PAGEDIRS, TRUE);
	if (pd == NULL)
		return -E_NOMEM;
	memset(pd, 0, sizeof(pd) * PAGEDIRS);

	/**
	* Get the start and end address of the total image with heap
	*/
	volatile addr_t base_addr = offset;
	volatile addr_t img_end = (addr_t)&end;
#ifdef PAGEDBG
	printf("Absolute size in bytes: %X\n", (img_end - base_addr));
#endif

	/**
	* Configure the page tables to point to the absolute image address space.
	*/
	addr_t idx;
	addr_t kern_size = 0xE00000;
	addr_t phys_start = base_addr - offset;

	for (idx = 0; idx <= kern_size; idx += PAGESIZE)
	{
#ifdef PAGEDBG
		printf("Virtual: %X\tPhysical: %X\tEntry: %X\n",
                                         base_addr+idx, phys_start+idx, idx);
#endif

		int err1 = page_map_entry(base_addr+idx, phys_start+idx, pd, false);
		int err2 = page_map_entry(idx, phys_start+idx, pd, false);
		int err3 = -E_SUCCESS;
#ifdef PAGEDBG
		printf("%X\t%X\n", err1, err2);
#endif

		if (idx_kernel_space == MAP_NOMAP)
			idx_kernel_space = map_set_page(idx_kernel_space, idx);
		else
			err3 = map_set_page(idx_kernel_space, idx/PAGESIZE);

		if (err1 != -E_SUCCESS || err2 != -E_SUCCESS || err3 != -E_SUCCESS)
		{
			printf("Error code 1: %X\nError code 2: %X\n", err1, err2);
			printf("Error code 3: %X\n", err3);
			panic("Paging fails epicly!!!");
		}
	}
  #ifdef PAGEDBG
  printf("Base addr: %X\tStart addr: %X\tlist_start: %X\n",
                                        (addr_t)&begin, (addr_t)&begin - offset,
                                                              idx_kernel_space);
  #endif
  return (addr_t)pd;
}

int page_unmap_low_mem()
{
	addr_t idx = 0;
	addr_t kern_size = 0xE00000;
	for (; idx < kern_size; idx += PAGESIZE)
	{
		page_release_entry(idx, (void*)getPageDir());
	}
	return -E_SUCCESS;
}

int page_alloc_page(uint32_t list_idx, addr_t virt_addr, struct page_dir *pd,
                                                               boolean userMode)
{
	addr_t phys_addr = map_alloc_page(list_idx);
	if (phys_addr == (addr_t) (-E_BMP_NOMEM))
	{
		printf("Map error code: %X\n", (uint32_t)(phys_addr >> 12));
		return -E_PAGE_NOMEM;
	}

	int map = page_map_entry(virt_addr, phys_addr, pd, userMode);
	if (map != -E_SUCCESS)
	{
		map_rm_page(phys_addr);
		return map;
	}
	return map;
}

addr_t page_phys_addr(addr_t virt, struct page_dir *pd)
{
	uint32_t directory_idx = virt >> 22;
	uint32_t table_idx = (virt >> 12) & 0x3FF;

	if (virt_page_dir[directory_idx] == 0)
		return ~0;

	struct page_table* pt= (void*)virt_page_dir[directory_idx];
	addr_t phys = pt[table_idx].pageIdx << 12;
	phys += (virt & PAGE_BITS);
#ifdef PAGEDBG
	printf("dir: %X\ttable: %X\n", directory_idx, table_idx);
	printf("dir idx: %X\ttable idx: %X\n", pd[directory_idx].pageIdx,
								pt[table_idx].pageIdx);
	printf("dir addr: %X\ttable addr: %X\n", (addr_t)pd, (addr_t)pt);
#endif
	return phys;
}

extern int init(unsigned long, void*);

void page_init()
{
	memset(page_cnt, 0, PAGETABLES*sizeof(uint16_t));
	memset(virt_page_dir, 0, PAGETABLES*sizeof(addr_t));
	task_init();
	addr_t tmp = setup_page_dir();
#ifdef PAGEDBG
	printf("Image start: %X\tStart ptr: %X\n", &begin, &init);
	printf("CR3: %X\tphys start ptr: %X\tActual start: %X\n",
				       tmp-offset, page_phys_addr((addr_t)&init,
					     (void*)tmp), (addr_t)&init-offset);
	printf("Pt_allocs: %X\tPt_uses: %X\n", pt_allocs, pt_uses);
	printf("Kern IDX: %X\n", idx_kernel_space);
	map_show_list(idx_kernel_space);
#endif
	setCR3(((addr_t)(tmp-offset)) & 0xFFFFF000);
	setPGBit();
}
