#!/bin/bash
cd `dirname $0`
./compile.sh $@
RET=$?
if [ ${RET} -eq 0 ]; then
./updatefloppy.sh
kvm -fda floppy.img -m 32M -s
else
echo " "
echo -en '\E[1;31m'"\033[1m     ***   $RET Errors found!     ***\033[0m"
echo " "
fi
exit ${RET}
