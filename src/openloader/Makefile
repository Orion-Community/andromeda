# Tools
AS=nasm
LD=ld
CC=gcc
MAKE=make

# Flags
ASFLAGS=-felf32 -I"include/" $(FLAGS)
BINARYASFLAGS=-f bin -I"include/" $(FLAGS)
CCFLAGS=-c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin \
-finline-functions-called-once -fno-weak -nostdinc -Iinclude $(FLAGS)
LDFLAGS=-Tlink.ld --oformat binary -melf_i386
FLAGS=-D __HDD -D __DEBUG

# Deps
BOOTBLOCK_DEPS=boot.o kern.o arch.o mm.o sys.o video.o error.o

# Images
MBR=masterboot.bin
BIN1=stage1.bin
TEMPBIN=temp.bin
BOOTBLOCK=openloader.bin
TESTIMG=testimage.bin

.PHONY: all
all: x86

.PHONY: x86
x86: $(BOOTBLOCK)
	

$(BOOTBLOCK):
	$(MAKE) FLAGS="$(FLAGS)" -C boot x86
	$(MAKE) FLAGS="$(FLAGS)" -C arch x86
	$(MAKE) FLAGS="$(FLAGS)" -C kern
	$(MAKE) FLAGS="$(FLAGS)" -C error
	$(MAKE) FLAGS="$(FLAGS)" -C sys
	$(MAKE) FLAGS="$(FLAGS)" -C mm
	$(MAKE) FLAGS="$(FLAGS)" -C video
	mv -v boot/boot.o ./
	mv -v boot/masterboot.bin ./$(MBR)
	mv -v boot/stage1.bin ./$(BIN1)
	mv -v arch/arch.o ./
	mv -v sys/sys.o ./
	mv -v error/error.o ./
	mv -v mm/mm.o ./
	mv -v kern/kern.o ./
	mv -v video/video.o ./
	$(LD) $(LDFLAGS) $(BOOTBLOCK_DEPS) -o $(TEMPBIN)
	@echo "\n"
	@echo "The openLoader project has been builded for the intel x86 architecture."
	@echo "Issue a make install to complete the process."

.PHONE: install
install: $(BIN1) $(TEMPBIN)
	mkdir -v build/
	mv -v $(MBR) build/
	dd if=$(BIN1) of=build/$(BOOTBLOCK) seek=0
	dd if=$(TEMPBIN) of=build/$(BOOTBLOCK) seek=5 ibs=512 conv=sync
	@echo "\n"
	@echo "Thank you for using the openLoader project, \n OpenLoader development team"

.PHONE: test
test: install
	dd if=build/$(MBR) of=build/$(TESTIMG) seek=0
	dd if=build/$(BOOTBLOCK) of=build/$(TESTIMG) seek=2048

.PHONY: clean
clean:
	$(MAKE) -C boot clean
	$(MAKE) -C arch clean
	$(MAKE) -C kern clean
	$(MAKE) -C error clean
	$(MAKE) -C sys clean
	$(MAKE) -C mm clean
	$(MAKE) -C video clean
	rm -v *.o *.bin
	rm -Rfv build/
	@echo "\nWorking directory is clean"

#$(BOOTBLOCK): $(BIN1) $(BIN2)
#	dd if=$(BIN1) of=$(BOOTBLOCK) seek=0
#	dd if=$(BIN2) of=$(BOOTBLOCK) seek=1 ibs=512 conv=sync

#$(OL): $(MBR_IMG) $(BOOTBLOCK)
#	dd if=$(MBR_IMG) of=$(OL) seek=0
#	dd if=$(BOOTBLOCK) of=$(OL) seek=2048

