#!/bin/bash

# Pull in config settings
source 000_config.sh

# Our tweaked login.defs
[ -d $ROOTDEST/etc ] || mkdir -p $ROOTDEST/etc 
cp $SCRIPTS/etc/login.defs $ROOTDEST/etc/
