#!/bin/bash

sudo /sbin/losetup /dev/loop0 loader.bin
sudo bochs -f bochsrc.txt
sudo /sbin/losetup -d /dev/loop0
