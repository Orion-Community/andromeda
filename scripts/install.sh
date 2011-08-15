#!/bin/bash

mbr="../src/build/masterboot.bin"
bootblock="../src/build/openloader.bin"

if test $# -le 1
then
	echo "Usage: $0 [device] [image]"
	echo "The openLoader will be writter to the\
 device you specified. It will cause\
 data loss."
	echo "Specify masterboot as image to\
 write the masterboot to the specified drive.\
 Enter bootblock to write the main image to the device."
else
	if [ ! -f $mbr ] || [ ! -f $bootblock ]
	then
		echo "Compile the project first."
	else if test $2 = "masterboot"
	then
		sudo fdisk -u $1
		sudo dd if=$mbr of=$1 count=446 bs=1 seek=0 skip=0
		sudo dd if=$mbr of=$1 seek=510 skip=510 count=2 bs=1
	else if test $2 = "bootblock"
	then
		sudo dd if=$bootblock of=$1 bs=1 skip=0 seek=0
	fi
	fi
	fi
fi

exit 0

