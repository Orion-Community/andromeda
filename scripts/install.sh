#!/bin/bash

mbr="../src/build/masterboot.bin"
bootblock="../src/build/openloader.bin"

if test $# -le 1
then
	echo "Usage: $0 [device] [image]"
	echo "The openLoader will be writter to the\
 device you specified. It will cause\
 data loss."
else
	if [ ! -f $mbr ] || [ ! -f $bootblock ]
	then
		echo "Compile the project first."
	else if test $2 = "masterboot"
	then
		sudo fdisk -u -C17 -H16 -S63 $1
		sudo dd if=$mbr of=$1 count=1 bs=445 seek=0
		sudo dd if=$mbr of=$1 count=1 bs=2 seek=510
	else if test $2 = "bootblock"
	then
		sudo dd if=$bootblock of=$1 ibs=512 seek=0
	fi
	fi
	fi
fi

exit 0

