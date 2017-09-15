#!/bin/bash

# Pull in config settings
source ../files/init/config.sh

# Our tweaked login.defs
[ -d $ROOTDEST/etc ] || mkdir -p $ROOTDEST/etc 
cp $FILES/etc/login.defs $ROOTDEST/etc/
