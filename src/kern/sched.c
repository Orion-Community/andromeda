#include <kern/sched.h>

struct machineState
{
	int eax, ebx, ecx, edx;
	int ebp, esp, ss, ds;
	int eip, eflags;
}
struct taskHdr
{
	int pid;
	int gid;
	int uid;
	struct machineState state;
	int pageTable;
}

void sched()
{
}

void fork(int pid)
{
}

void kill (int pid)
{
}
