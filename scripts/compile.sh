#!/bin/bash
s=0
for var in $@
do
	if [ ${var:0:1} = "-" ];
	then
		case "${var:1}" in
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
make FLAGS="$FLAGS" DEFS="$DEFS" CC=distcc -B $DEFS
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
