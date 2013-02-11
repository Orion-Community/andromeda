#!/bin/bash
cp andromeda.img scripts/iso/boot/

echo $(pwd)

if ! command -v grub-mkrescue -o andromeda.iso scripts/iso &>/dev/null; then
	grub2-mkrescue -o andromeda.iso scripts/iso
fi
