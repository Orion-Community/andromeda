#!/bin/bash
if [ ! -f scripts/build.jar ]; then
	wget http://ci.orion-os.eu/job/Build/lastSuccessfulBuild/artifact/target/build.jar -O scripts/build.jar;
fi
