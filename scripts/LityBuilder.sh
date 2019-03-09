#!/bin/bash

LITY="/mnt/d/User/Documents/Git/EthBrenchmark/bin/lity-v1.2.4-static"
BUILDFILE=$1

$LITY --version
$LITY $BUILDFILE --bin-runtime -o ./ --overwrite
