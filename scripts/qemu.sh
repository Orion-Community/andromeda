#!/bin/bash
export KERN_PATH=bin/andromeda.img

if ! command -v qemu-system-i386; then
	qemu -kernel $KERN_PATH -monitor stdio -m 64M;
else
	qemu-system-i386 -kernel $KERN_PATH -monitor stdio -m 64M;
fi
