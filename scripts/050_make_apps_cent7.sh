#!/bin/bash
#
# Create /apps/cent7 for things we need to install

# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# Make it so! Create directory in shared filesystem, symlink from /apps
mkdir -p $ROOTDEST/shared/apps/cent7
ln -s --force -t $ROOTDEST  ${ROOTDEST%%/}/shared/apps 
