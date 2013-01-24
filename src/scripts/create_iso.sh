#!/bin/bash
cp andromeda.img scripts/iso/boot/

echo $(pwd)

grub-mkrescue -o andromeda.iso scripts/iso
