#!/bin/bash
#
# Make a /scratch symlink and prepare everything for personal scratches 
# under /shared/scratch

# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# Make sure shared scratch exits and has proper wide-open permissions :)
# Would have never done this in real life, but hey, we are not making a 
# production scale system here, so it's ok among friends.
SCRATCH_BASE=/shared/scratch
[[ -d $SCRATCH_BASE ]] || mkdir -p $SCRATCH_BASE
chmod 1777 $SCRATCH_BASE

# Make top-level scratch link
ln -s --force -t $ROOTDEST  ${ROOTDEST%%/}/${SCRATCH_BASE##/}  # Careful with /

# Create rcac-scratch.sh
[[ -d /etc/profile.d ]] || mkdir -p $ROOTDEST/etc/profile.d
cp $SCRIPTS/etc/profile.d/rcac-scratch.sh  $ROOTDEST/etc/profile.d
cp $SCRIPTS/etc/profile.d/rcac-scratch.csh $ROOTDEST/etc/profile.d
