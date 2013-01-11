#!/bin/bash
if [[ $EUID -ne 0 ]]; then
	echo "Do it your self!";
	exit 0;
else
	echo "Ok";
	exit 0;
fi
