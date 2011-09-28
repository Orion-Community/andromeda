#!/bin/bash

function open
{
	cd ../
	for var in $@
	do
		kate `find -name "${var}"`
	done
}

function explain
{
	echo "You should use this script as following:"
	echo
	echo "$0 <[FILE] | [PATTERN]>"
}

[[ $# -eq 0 ]] && explain

[[ $# -eq 1 ]] && open $@
