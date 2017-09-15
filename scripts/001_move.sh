#!/bin/bash

source ../files/init/config.sh
[ -d $ROOTDEST/etc ] || mkdir -p $ROOTDEST/etc 
cp $FILES/etc/login.defs $ROOTDEST/etc/
