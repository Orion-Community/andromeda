#!/bin/bash
n=0
function options {
	case "$1" in
		h|-help)
			echo "Usage:";
			echo "       debug.sh <options> [make and c header defines...]";
			echo "Options:";
			echo "       -h --help     Print this help message";
			echo "       -n --nosudo   script will not use sudo for mound/unmounting. Requires ";
			echo "                     mounted with -n option."
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
sudo umount /dev/loop0
else
umount /media/loop
fi
