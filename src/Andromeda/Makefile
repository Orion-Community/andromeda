include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

ELF=elf.c exec.c
TIMER=timer.c
SCHED=sched.c
CORE=core.c
SYSCALL=syscall.c

.PHONY: all
all: andromeda.o
	mv -fv andromeda.o ../

andromeda.o: load.o timer.o sched.o core.o syscall.o
	$(LD) $(LDFLAGS) *.o -o andromeda.o

load.o: $(ELF)
	$(CC) $(CFLAGS) $(FLAGS) $(ELF)

timer.o: $(TIMER)
	$(CC) $(CFLAGS) $(FLAGS) $(TIMER)

sched.o: $(SCHED)
	$(CC) $(CFLAGS) $(FLAGS) $(SCHED)

core.o: $(CORE)
	$(CC) $(CFLAGS) $(FLAGS) $(CORE)

syscall.o: $(SYSCALL)
	$(CC) $(CFLAGS) $(FLAGS) $(SYSCALL)

.PHONY: clean
clean:
	rm -fv *.o