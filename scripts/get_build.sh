#!/bin/bash
echo "Fetching build system"
#Fetch build itself
BUILD_VERSION=-0.2
BUILD_DOWNLOAD=$BUILD_VERSION

for arg in $@;
do
	case $arg in
		-testing)
			BUILD_DOWNLOAD=-testing;
			;;
		-bemk)
			BUILD_DOWNLOAD=-bemk;
			;;
	esac
done

BUILD_URL=http://ci.orion-os.eu/job/Build$BUILD_DOWNLOAD/lastSuccessfulBuild/artifact/build\ -O\ scripts/build$BUILD_VERSION;

if [ ! -f scripts/build$BUILD_VERSION ]; then
	wget $BUILD_URL
	chmod +x scripts/build$BUILD_VERSION
fi

#Make sure the symlink is correct
if [ -f scripts/build ]; then
	rm scripts/build
fi
ln -sv build$BUILD_VERSION scripts/build

