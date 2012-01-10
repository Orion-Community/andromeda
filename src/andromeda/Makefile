include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

ELF=elf.c exec.c
TIMER=timer.c
SCHED=sched.c
CORE=core.c
SYSCALL=syscall.c
PANIC=panic.c

.PHONY: all
all: andromeda.o
	mv -fv andromeda.o ../

andromeda.o: load.o timer.o sched.o core.o syscall.o panic.o drivers.o buffer.o
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

panic.o: $(PANIC)
	$(CC) $(CFLAGS) $(FLAGS) $(PANIC)

drivers.o: drivers.c
	$(CC) $(CFLAGS) $(FLAGS) drivers.c

buffer.o: buffer.c
	$(CC) $(CFLAGS) $(FLAGS) buffer.c

.PHONY: clean
clean:
	rm -fv *.o