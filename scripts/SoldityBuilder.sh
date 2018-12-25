#!/bin/bash

SOLDITYC="/mnt/d/User/Documents/Git/EthBrenchmark/bin/solc-static-linux"
BUILDFILE=$1
OUTNAME=$2

$SOLDITYC --version
$SOLDITYC $BUILDFILE --bin-runtime -o ./ --overwrite
mv *.bin-runtime $OUTNAME