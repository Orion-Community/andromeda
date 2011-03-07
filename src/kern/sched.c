#include <kern/sched.h>
#include <error/panic.h>

struct machineState
{
	int eax, ebx, ecx, edx;
	int ebp, esp, ss, ds;
	int eip, eflags;
};
struct taskHdr
{
	int pid;
	int gid;
	int uid;
	struct machineState state;
	int pageTable;
};

void sched()
{
	panic();
}

void fork(int pid)
{
	panic();
}

void kill (int pid)
{
	panic();
}
