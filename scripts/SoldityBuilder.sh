#!/bin/bash

BUILDFILE=$1

if [ -z ${SOLIDITYC+x} ]; then
SOLIDITYC="solc"
fi

$SOLIDITYC --version
$SOLIDITYC $BUILDFILE --bin-runtime -o ./ --overwrite
