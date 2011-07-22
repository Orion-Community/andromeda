#!/bin/bash
cd `dirname $0`/..
FLAGS="-O3"
DEFS=""
for i in $*
do
    FLAGS="$FLAGS -D $i"
    DEFS="$DEFS $i=enabled"
done
make clean -B $DEFS
RET=$?
if [ ${RET} -eq 0 ]; then
make FLAGS="$FLAGS" CC=distcc -B $DEFS
RET=$?
exit $RET
else
echo -en '\E[1;31m'"\033[1m   !!!   Some weird error happend ($RET errors during clean)   !!!\033[0m"
exit $RET
fi
