#!/bin/bash
n=""
for var in $@
do
	if [ ${var:0:1} = "-" ];
	then
		case "${var:1}" in
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
	fi
done
D=`dirname $0`
$D/setupfloppy.sh$n
$D/cpfloppy.sh
$D/releasefloppy.sh$n
