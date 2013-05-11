#!/bin/bash
export KERN_PATH=bin/andromeda.img
export MEM_SIZE="64M"

export QEMU_FLAGS="-kernel $KERN_PATH -m $MEM_SIZE -monitor stdio"

if ! command -v qemu-kvm; then
	if ! command -v qemu-system-i386; then
		qemu $QEMU_FLAGS
	else
		qemu-system-i386 $QEMU_FLAGS
	fi;
else
	qemu-kvm $QEMU_FLAGS
fi
