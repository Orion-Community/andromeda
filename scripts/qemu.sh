#!/bin/bash
export KERN_PATH=bin/andromeda.img

if ! command -v qemu-system-i386; then
	qemu -kernel $KERN_PATH -m 64M -monitor stdio;
else
	qemu-system-i386 -kernel $KERN_PATH -m 64M -monitor stdio ;
fi
