#!/bin/bash
s=0
function options {
	case "$1" in
		h|-help)
			echo "Usage:";
			echo "       compile.sh <options> [make and c header defines...]";
			echo "Options:";
			echo "       -h --help     Print this help message";
			echo "       -s --silent   Silent mode (script will print makefile returns only)";
			exit 0 ;;
		s|-silent)
			s=1 ;;
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
cd `dirname $0`/..
FLAGS=""
DEFS=""
for i in $*
do
    FLAGS="$FLAGS -D $i"
    DEFS="$DEFS $i=enabled"
done
make clean -B $DEFS
RET=$?
if [ ${RET} -eq 0 ]; then
make FLAGS="$FLAGS" DEFS="$DEFS" -B $DEFS -j 2
RET=$?
exit $RET
else
if [ $s -eq 0 ];
then
echo ""
echo -en '\E[1;31m'"\033[1m   !!!   Some weird error happend ($RET errors during clean)   !!!\033[0m\n"
echo ""
fi
exit $RET
fi
