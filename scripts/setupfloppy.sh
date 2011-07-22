#!/bin/bash
sudo losetup /dev/loop0 `dirname $0`/floppy.img
sudo mount /dev/loop0 /media/loop
