#!/usr/bin/env bash

OSDIR=$(pwd)/HDE/x86_64.linux

if [ ! -d "$OSDIR" ]; then
	echo "Downloading OpenSplice ..."
	wget http://www.joe.cloud/download/opensplice.tar.gz

	echo "Decompressing to directory: $(pwd) ..."
	tar xf opensplice.tar.gz

	echo "Deleting opensplice.tar.gz ..."
	rm opensplice.tar.gz

	echo "Updating release.com ..."
	sed -i 12s@.*@"OSPL_HOME="$OSDIR"@" $OSDIR/release.com
fi

echo "Sourcing release.com ..."
source $OSDIR/release.com

echo "Setting opensplice path ..."
export OSPL_HOME="$OSDIR"/

