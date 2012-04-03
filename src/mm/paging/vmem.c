#include <mm/x86/vmem.h>
#include <mm/paging.h>
#include <andromeda/error.h>

struct vmem_branch* lookup_tree;

int
vmem_init()
{
        if (lookup_tree != NULL)
                return -E_ALREADY_INITIALISED;
        lookup_tree = kalloc(sizeof(*lookup_tree));

        return -E_SUCCESS;
}

void*
phys_to_virt(void* phys)
{
        return NULL;
}