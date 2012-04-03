#ifndef __MM_X86_VMEM_H
#define __MM_X86_VMEM_H

#ifdef __cplusplus
extern "C" {
#endif

struct vmem_branch {
        union {
                struct vmem_branch* branches[16];
                void* virt_addr[16];
        };
};

#ifdef __cplusplus
}
#endif

#endif
