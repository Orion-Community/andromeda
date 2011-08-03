#!/bin/bash
n=""
function options {
	case "$1" in
		h|-help)
			echo "Usage:";
			echo "       debug.sh <options> [make and c header defines...]";
			echo "Options:";
			echo "       -h --help     Print this help message";
			echo "       -n --nosudo   script will not use sudo for mound/unmounting. Requires ";
			echo "                     `dirname $0`/floppy.img in fstab. (For more info type:"
			echo "                     `dirname $0`/setupfloppy.sh -h"
			exit 0 ;;
		n|-nosudo)
			n=" -n";;
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
D=`dirname $0`
$D/setupfloppy.sh$n
$D/cpfloppy.sh
$D/releasefloppy.sh$n
