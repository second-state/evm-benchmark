#!/bin/bash

BUILDFILE=$1
OUTNAME=$2

if [ -z ${SOLIDITYC+x} ]; then
SOLIDITYC="solc"
fi

$SOLIDITYC --version
$SOLIDITYC $BUILDFILE --bin-runtime -o ./ --overwrite
mv *.bin-runtime $OUTNAME