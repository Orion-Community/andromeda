#!/bin/bash
echo "Fetching build system"
BUILD_VERSION=-0.2
if [ ! -f scripts/build$BUILD_VERSION ]; then
	wget http://ci.orion-os.eu/job/Build$BUILD_VERSION/lastSuccessfulBuild/artifact/build -O scripts/build$BUILD_VERSION;
	if [ -f scripts/build ]; then
		rm scripts/build
	fi
	ln -sv build$BUILD_VERSION scripts/build
	chmod +x scripts/build$BUILD_VERSION
fi
