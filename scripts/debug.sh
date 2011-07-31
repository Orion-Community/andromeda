#!/bin/bash
s=0
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
				echo "       -s --silent   Silent mode (script wont print anything)";
				exit 0 ;;
			s|--silent)
				s=1 ;;
			*)
				echo "Unknown option ${var}";
				exit 0;;
		esac
	else
		OPT+=" $var"
	fi
done
cd `dirname $0`
./compile.sh $OPT
RET=$?
if [ ${RET} -eq 0 ]; then
./updatefloppy.sh
kvm -fda floppy.img -m 32M -s
else
if [ $s -eq 0 ];
then
echo ""
echo -en '\E[1;31m'"\033[1m     ***   $RET Errors found!     ***\033[0m\n"
echo ""
fi
fi
exit ${RET}
