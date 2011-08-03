#!/bin/bash
n=0
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
				echo "                     mounted with -n option."
				exit 0 ;;
			n|-nosudo)
				n=1 ;;
			*)
				echo "Unknown option ${var}";
				exit 0;;
		esac
	fi
done
if [ $n -eq 0 ];
then
sudo umount /dev/loop0
sudo losetup -d /dev/loop0
else
umount /media/loop
fi
