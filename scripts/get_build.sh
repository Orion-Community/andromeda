#!/bin/bash
if [ ! -f scripts/build ]; then
	wget http://ci.orion-os.eu/job/Build-0.2/lastSuccessfulBuild/artifact/build -O scripts/build;
	chmod +x scripts/build
fi
