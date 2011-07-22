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
make FLAGS="$FLAGS" CC=distcc -B $DEFS
