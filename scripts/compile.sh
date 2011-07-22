#!/bin/bash
cd `dirname $0`/..

function error { 
echo Failed. 
ls -l $SNAPSHOT | mail seanw -s "Backup script failed" 
exit 
}

FLAGS="-O3"
DEFS=""
for i in $*
do
    FLAGS="$FLAGS -D $i"
    DEFS="$DEFS $i=enabled"
done
make clean -B $DEFS
make FLAGS="$FLAGS" CC=distcc $DEFS
RET=$?
exit $RET
