#!/bin/bash
cd `dirname $0`
./compile.sh $@
./updatefloppy.sh
kvm -fda floppy.img -m 32M -s

