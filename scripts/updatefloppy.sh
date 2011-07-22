#!/bin/bash
D=`dirname $0`
$D/setupfloppy.sh
$D/cpfloppy.sh
$D/releasefloppy.sh
