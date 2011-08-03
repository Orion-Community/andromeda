#!/bin/bash
n=0
function options {
	case "$1" in
		h|-help)
			if [ "${0:0:1}" = "." ]
			then
				fullpath=${0:1}
				echo $fullpath
			else
				fullpath=$0
			fi
			if [ "${fullpath:0:1}" = "/" ]
			then
				fullpath=`dirname $PWD$fullpath`
			else
				fullpath=`dirname $PWD/$fullpath`
			fi
			echo "Usage:";
			echo "       debug.sh <options> [make and c header defines...]";
			echo "Options:";
			echo "       -h --help     Print this help message";
			echo "       -n --nosudo   script will not use sudo for mound/unmounting. Requires ";
			echo "                     `dirname $0`/floppy.img in fstab. (if not at this line to /etc/fstab:"
			echo "                     $fullpath/floppy.img /media/loop auto users,loop,noauto 0 0)"
			exit 0 ;;
		n|-nosudo)
			n=1 ;;
		*)
			echo "Unknown option ${var}";
			exit 0;;
	esac
}
for var in $@
do
	if [ "${var:0:1}" = "-" ];
	then
		if [ "${var:1:2}" = "-" ]
		then
			options "${var:1}";
		else
			count=1
			len=`expr length ${var}`
			while [ $count -lt $len ]
			do
				options "${var:$count:`expr $coutn+1`}";
				count=`expr $count + 1`
			done
		fi
	else
		OPT+=" $var"
	fi
done
if [ $n -eq 0 ];
then
sudo losetup /dev/loop0 `dirname $0`/floppy.img
sudo mount /dev/loop0 /media/loop
else
mount `dirname $0`/floppy.img
fi
