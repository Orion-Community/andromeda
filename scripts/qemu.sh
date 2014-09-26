#!/bin/bash
KERN_PATH=bin/andromeda.img
MEM_SIZE="64M"

CORE_DEV="-kernel bin/andromeda.img"

for arg in $@
do
	case $arg in
		-dbg)	
			ARG_FLAGS="$ARG_FLAGS -s -S"
			;;
		-cdrom)
			CORE_DEV="-cdrom bin/andromeda.iso"
			;;
		-serial)
			ARG_FLAGS="$ARG_FLAGS -serial file:serial_out.txt"
			;;
	esac
done

QEMU_FLAGS="$CORE_DEV -m $MEM_SIZE -monitor stdio $ARG_FLAGS"

echo $QEMU_FLAGS

if ! command -v qemu-kvm; then
	if ! command -v kvm; then
		if ! command -v qemu-system-i386; then
			qemu $QEMU_FLAGS
		else
			qemu-system-i386 $QEMU_FLAGS
		fi;
	else
		kvm $QEMU_FLAGS
	fi;
else
	qemu-kvm $QEMU_FLAGS
fi
