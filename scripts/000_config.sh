#!/bin/bash
#
# This file is sourced by all cloud-init scripts from ../scripts dir.
# Main idea is to define common settings and virtually no direct action.

# Where Cluster-Init files and scripts are.
# Relative path ('..') works fine for testing and production.
# Or could use production-quality absolute '/mnt/cluster-init', 
# but may have to change for off-site testing.
INIT=..
FILES=$INIT/files
SCRIPTS=$INIT/scripts/

# And where the root of the destination filesystem is.
# Things like '$INIT/../test' or '$HOME/test' are good for debugging.
# Obviously should be set to "/" for production.
ROOTDEST=$INIT/../test
if [ ! -d $ROOTDEST ]; then
     mkdir -p $ROOTDEST
fi

