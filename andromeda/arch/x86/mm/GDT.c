#ifdef __INTEL
#include <mm/memory.h>
#include <GDT.h>

#ifdef GDTTEST
#include <text.h>
#endif

// All this does is set the general descriptor table to a flat memory model.
// For all I know this is only necessary on intel machines as they support
// segmentation. We'll use paging for our memory protection.

void setGDT()
{
  #ifdef GDTTEST
  printf("GDT ptr size\tGDT Entry size\n");
  printhex(sizeof(gdt_t)); printf("\t\t");
  printhex(sizeof(gdtEntry_t)); putc('\n');
  #endif
  
  /*
   * Set up a GDT. (1)
   */
  
  /*
   * Load the GDT into the CPU.(1)
   */
}

#endif

/*
(1) A GDT or general descriptor table is a table that holds the segment data.
    To get to know more about the data, see GDT.h.
    Whenever an application does a memory reference it is passed by the GDT to
    figure out to which address the address given should be mapped.
    The addresses on the intel machine are built up out of segment registers
    and the pointer itself.
    
    The segment registers hold an index into the GDT.
*/