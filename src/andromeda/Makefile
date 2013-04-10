include ../scripts/makeIncl
CFLAGS+=-I../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=scheduling.o sched.o core.o syscall.o panic.o drivers.o buffer.o atomic.o lib.o \
load.o ktimer.o

ELF=elf.c exec.c
TIMER=timer.c
CORE=core.c
SYSCALL=syscall.c
PANIC=panic.c

MK_FLAGS=FLAGS="$(FLAGS)"

.PHONY: all
all: andromeda.o
	mv -fv andromeda.o ../

andromeda.o: $(TARGETS) ktimer.o
	$(LD) $(LDFLAGS) *.o -o andromeda.o

load.o: $(ELF)
	$(CC) $(CFLAGS) $(FLAGS) $(ELF)

ktimer.o:
	$(MAKE) -C timer/ $(MK_FLAGS)

scheduling.o:
	$(MAKE) -C scheduling/ $(MK_FLAGS)

sched.o:
	$(MAKE) -C sched/ $(MK_FLAGS)

.PHONY: clean
clean:
	rm -fv *.o
