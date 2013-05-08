#!/bin/bash
cp bin/andromeda.img scripts/iso/boot/

echo $(pwd)

export ISO_PATH=bin/andromeda.iso
export ISO_SRC=scripts/iso/

if ! command -v grub-mkrescue &>/dev/null; then
	grub2-mkrescue -o $ISO_PATH $ISO_SRC
else
	grub-mkrescue -o $ISO_PATH $ISO_SRC
fi
