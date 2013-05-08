#!/bin/bash
export KERN_PATH=bin/andromeda.iso

if ! command -v qemu-system-i386 &> /dev/null; then
	qemu -cdrom $KERN_PATH -monitor stdio -m 64M
else
	qemu-system-i386 -cdrom $KERN_PATH -monitor stdio -m 64M
fi
