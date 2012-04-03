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

struct vmem_branch*
vmem_branch_init()
{
        struct vmem_branch* ret = kalloc(sizeof(*ret));
        if (ret == NULL)
                return NULL;

        memset(ret, 0, sizeof(*ret));
        return ret;
}

#define MAP_DEPTH 4
#define FOUR_BITS 0xF
#define TWELVE_BITS 0x3FF

int
vmem_map(void* phys, void* virt)
{
        /** Check the arguments */
        if (phys == NULL || virt == NULL)
                return -E_NULL_PTR;

        if (lookup_tree == NULL)
                return -E_NOT_YET_INITIALISED;

        if ((addr_t)phys & TWELVE_BITS != 0 || (addr_t)virt & TWELVE_BITS != 0)
                return -E_INVALID_ARG;

        /** Now find the branch to work with */
        int i = 0;
        addr_t phys_addr = (addr_t)phys;
        phys_addr >>= 12;
        struct vmem_branch* carriage = lookup_tree;
        register int branch_idx = 0;
        for (; i < MAP_DEPTH; i++)
        {
                branch_idx = phys_addr & FOUR_BITS;
                phys_addr >>= 4;
                if (carriage->branches[branch_idx] == NULL)
                        carriage->branches[branch_idx] = vmem_branch_init();
                carriage = carriage->branches[branch_idx];
        }

        /** Now finally set the leaf ... */
        branch_idx = phys_addr & FOUR_BITS;
        carriage->virt_addr[branch_idx] = virt;

        return -E_SUCCESS;
}

int vmem_map_region(void* phys, void* virt, size_t size)
{
        if (phys == NULL || virt == NULL)
                return -E_NULL_PTR;
        if (lookup_tree == NULL)
                return -E_NOT_YET_INITIALISED;
        if (!((addr_t)phys & TWELVE_BITS) || !((addr_t)virt & TWELVE_BITS) ||
                                                        !(size & TWELVE_BITS))
                return -E_INVALID_ARG;

        int idx = 0;
        for (; idx < size; idx += PAGE_SIZE)
        {
                int ret = vmem_map((void*)((addr_t)phys+idx),(void*)((addr_t)virt+idx));
                if (ret != -E_SUCCESS)
                        return ret;
        }
        return -E_SUCCESS;
}

void*
phys_to_virt(void* phys)
{
        return NULL;
}