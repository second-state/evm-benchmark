#!/bin/bash

BUILDFILE=$1
OUTNAME=$2

if [ -z ${SOLDITYC+x} ]; then
SOLDITYC="solc"
fi

$SOLDITYC --version
$SOLDITYC $BUILDFILE --bin-runtime -o ./ --overwrite
mv *.bin-runtime $OUTNAME