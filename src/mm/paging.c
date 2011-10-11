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
#include <error/error.h>

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
addr_t idx_memory_mapped_io;
addr_t idx_kernel_code;
addr_t idx_kernel_heap;
addr_t idx_kernel_bss;
addr_t idx_kernel_stack;

/**
 * The andromeda paging system is set up here.
 * 
 * This does come with the warning that this isn't finished by a very very very
 * long shot. Feel free to contribute.
 */

/**
 * Called when a pagefault occurs, is in charge of fixing the fault and swapping
 * if necessary.
 */
void cPageFault(isrVal_t registers)
{
  unsigned long page = getCR2();
  #ifdef PAGEDBG
  printf("PG\n");
  #endif

  if (registers.cs != 0x8 && registers.cs != 0x18)
  {
    panic("Incorrect frame!");
  }
  #ifdef PAGEDBG
  printf("Type of error: 0x%X\n", registers.errCode);
  #endif

  panic("Page faults currently under construction");
}

/**
 * A bitmap of the ammount of pages in use in every pagetable
 */
uint16_t page_cnt[PAGETABLES];

/**
 * Used to map a page to a physical addess, with the option to put it in
 * usermode and to choose a different page directory than the current (usefull
 * when using multiple processors).
 */
int page_map_entry(addr_t virtual, addr_t physical, struct page_dir *pd,
                                                               boolean userMode)
{
  while(mutexTest(page_lock))
  {
    #ifdef PAGEDBG
    printf("Paging is locked!\n");
    #endif
  }
  struct page_table* pt;
  addr_t pd_entry = virtual >> 22;
  addr_t pt_entry = (virtual >> 12) & 0x3FF;

  if (pd[pd_entry].present == FALSE)
  {
    pt = alloc(sizeof(pt)*PAGETABLES, TRUE);
    if (pt == NULL)
    {
      mutexRelease(page_lock);
      return -E_NOMEM;
    }
    memset(pt, 0, sizeof(pt)*PAGETABLES);
    pd[pd_entry].pageIdx  = ((addr_t)pt)/PAGESIZE;
    pd[pd_entry].present  = TRUE;
    pd[pd_entry].rw       = TRUE;
    pd[pd_entry].userMode = userMode;
  }
  else
  {
    if (pd[pd_entry].userMode == FALSE)
      pd[pd_entry].userMode = userMode;

    pt = (struct page_table*)(pd[pd_entry].pageIdx*PAGESIZE);
    if (pt == NULL)
    {
      mutexRelease(page_lock);
      return -E_PAGE_MAPPING;
    }
  }
  if (pt[pt_entry].present != FALSE)
  {
    mutexRelease(page_lock);
    return -E_PAGE_MAPPING;
  }

  pt[pt_entry].pageIdx  = physical >> 0x12;
  pt[pt_entry].present  = TRUE;
  pt[pt_entry].rw       = TRUE;
  pt[pt_entry].userMode = userMode;

  page_cnt[pd_entry]++;
  mutexRelease(page_lock);
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
  addr_t pt_entry = virtual >> 12;

  if (pd[pd_entry].present == FALSE)
  {
    mutexRelease(page_lock);
    return -E_PAGE_NOPAGE;
  }

  struct page_table *pt = (struct page_table*)(pd[pd_entry].pageIdx*PAGESIZE);

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
    free((void*)(pd[pd_entry].pageIdx*PAGESIZE));
    pd[pd_entry].pageIdx = 0;
  }
  mutexRelease(page_lock);
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
  struct page_dir *pd = alloc(sizeof(pd)*PAGEDIRS, TRUE);
  if (pd == NULL)
    return -E_NOMEM;
  memset(pd, 0, sizeof(pd)*PAGEDIRS);

  /**
   * Get the start and end address of the total image with heap
   */
  volatile addr_t base_addr = (addr_t)&mboot % PAGESIZE;
  volatile addr_t abs_end = ((addr_t)&end + (addr_t)HEAPSIZE);
  abs_end += (PAGESIZE-(((addr_t)&end + (addr_t)HEAPSIZE)%PAGESIZE));
  #ifdef PAGEDBG
  printf("Absolute size in bytes: %X\n", (abs_end - base_addr));
  int i = 0;
  #endif

  /**
   * Configure the page tables to point to the absolute image address space.
   */
  addr_t idx;
  for (idx = base_addr; idx < abs_end; idx += PAGESIZE)
  {
    #ifdef PAGEDBG
    i++;
    printf("Set page %X\n", i);
    #endif
  }

  return (addr_t)pd;
}

int page_copy_image(addr_t from, size_t size, addr_t to)
{
  return -E_NOFUNCTION;
}

int page_alloc_page(uint32_t list_idx, addr_t virt_addr, struct page_dir *pd,
                                                               boolean userMode)
{
  addr_t phys_addr = map_alloc_page(list_idx);
  if (phys_addr != (addr_t)(-E_SUCCESS)) return -E_PAGE_NOMEM;

  int map = page_map_entry(virt_addr, phys_addr, pd, userMode);
  if (map != -E_SUCCESS)
  {
    map_rm_page(phys_addr);
    return map;
  }
}

addr_t page_phys_addr(addr_t virt, struct page_dir *pd)
{
  int directory_idx = virt >> 22;
  int table_idx = (virt >> 12) & 0x3FF;

  struct page_table* pt = (void*)(pd[directory_idx].pageIdx << 12);
  addr_t phys = pt[table_idx].pageIdx << 12 | (virt & 0x3FF);
  return phys;
}

void page_init()
{
  memset(page_cnt, 0, PAGETABLES*sizeof(uint16_t));
  sched_init();
  setCR3(setup_page_dir());
//   setPGBit();
}