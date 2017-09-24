#!/bin/bash
#
# Install modulefiles for Lmod.
#
# Quick and simple - just copy the entire tree into /opt/modulefiles.
# Some of the modulefiles can thus end up in place before their corresponding
# /apps/cent7/.... installation dirs, but that's ok.
#
# Note also the use of .gitkeep file in some of the modulefiles subdirs
# (since git ignores empty directories, we need this stub to ensure directory
# creation).

# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# And copy modulefiles hierarchy to /opt/modulefiles
cp -r $SCRIPTS/modulefiles  $ROOTDEST/opt
