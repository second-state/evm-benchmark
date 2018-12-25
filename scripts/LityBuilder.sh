#!/bin/bash

LITY="/mnt/d/User/Documents/Git/EthBrenchmark/bin/lity-v1.2.4-static"
BUILDFILE=$1
OUTNAME=$2

$LITY --version
$LITY $BUILDFILE --bin-runtime -o ./ --overwrite
mv *.bin-runtime $OUTNAME