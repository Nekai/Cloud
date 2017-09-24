#!/bin/bash
#
# Make a /scratch symlink and prepare everything for personal scratches 
# under /shared/scratch

# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# Make top-level scratch link
ln -s --force -t $ROOTDEST  ${ROOTDEST%%/}/shared/scratch

# Create rcac-scratch.sh
mkdir -p $ROOTDEST/etc/profile.d
cp $SCRIPTS/etc/profile.d/rcac-scratch.sh  $ROOTDEST/etc/profile.d
cp $SCRIPTS/etc/profile.d/rcac-scratch.csh $ROOTDEST/etc/profile.d
