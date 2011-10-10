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
volatile addr_t offset = 0xC0000000;

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
#ifdef PAGEDBG
uint32_t pt_allocs = 0;
uint32_t pt_uses = 0;
#endif

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
  addr_t pd_entry = virtual >> 22 % 0x400;
  addr_t pt_entry = (virtual >> 12) % 0x400;

  if (pd[pd_entry].present == FALSE)
  {
#ifdef PAGEDBG
    pt_allocs++;
#endif
    pt = alloc(sizeof(pt)*PAGETABLES, TRUE);
    if (pt == NULL)
    {
      mutexRelease(page_lock);
      return -E_NOMEM;
    }
    memset(pt, 0, sizeof(pt)*PAGETABLES);
    pd[pd_entry].pageIdx  = ((addr_t)(pt)-offset) >> 12;
    pd[pd_entry].present  = TRUE;
    pd[pd_entry].rw       = TRUE;
    pd[pd_entry].userMode = TRUE;
  }
  else
  {
#ifdef PAGEDBG
    pt_uses++;
#endif
    pd[pd_entry].userMode = TRUE;

    pt = (void*)((pd[pd_entry].pageIdx << 12) + offset);
    if (pt == NULL)
    {
      mutexRelease(page_lock);
      printf("nomem\n");
      return -E_PAGE_MAPPING;
    }
  }
  if (pt[pt_entry].present == TRUE)
  {
    mutexRelease(page_lock);
    printf("Page already mapped! Unmap it first\n");
    return -E_PAGE_MAPPING;
  }

  pt[pt_entry].pageIdx  = physical >> 12;
  pt[pt_entry].present  = TRUE;
  pt[pt_entry].rw       = TRUE;
  pt[pt_entry].userMode = userMode;

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
  addr_t phys_start = base_addr - offset /** lea phys_start, cs[base_addr] */;

  for (idx = 0; idx <= kern_size; idx += PAGESIZE)
  {
    #ifdef PAGEDBG
//     printf("Virtual: %X\tPhysical: %X\tEntry: %X\n",
//                                          base_addr+idx, phys_start+idx, idx);
    #endif

    int err = page_map_entry(base_addr+idx, phys_start+idx, pd, false);
    if (err != -E_SUCCESS)
    {
      printf("Error code: %X\n", err);
      panic("Paging fails epicly!!!");
    }
  }
  #ifdef PAGEDBG
  printf("Base addr: %X\tStart addr: %X\n",
                                       (addr_t)&begin, (addr_t)&begin - offset);
  #endif
  return (addr_t)pd;
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
  uint32_t directory_idx = virt >> 22;
  uint32_t table_idx = (virt >> 12) & 0x3FF;

  struct page_table* pt=(void*)(((pd[directory_idx].pageIdx) << 12) + offset);
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
  sched_init();
  addr_t tmp = setup_page_dir();
#ifdef PAGEDBG
  printf("Image start: %X\tStart ptr: %X\n", &begin, &init);
  printf("CR3: %X\tphys start ptr: %X\tActual start: %X\n",
                                       tmp-offset, page_phys_addr((addr_t)&init,
                                             (void*)tmp), (addr_t)&init-offset);
  printf("Pt_allocs: %X\t Pt_uses: %X\n", pt_allocs, pt_uses);
#endif
  setCR3((addr_t)(tmp-offset));
//   setPGBit();
  for (;;);
}