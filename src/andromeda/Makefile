include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=sched.o core.o syscall.o panic.o drivers.o buffer.o atomic.o lib.o \
load.o ktimer.o

ELF=elf.c exec.c
TIMER=timer.c
SCHED=sched.c
CORE=core.c
SYSCALL=syscall.c
PANIC=panic.c

.PHONY: all
all: andromeda.o
	mv -fv andromeda.o ../

andromeda.o: $(TARGETS) ktimer.o
	$(LD) $(LDFLAGS) *.o -o andromeda.o

load.o: $(ELF)
	$(CC) $(CFLAGS) $(FLAGS) $(ELF)

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

ktimer.o:
	$(MAKE) -C timer/

.PHONY: clean
clean:
	rm -fv *.o
