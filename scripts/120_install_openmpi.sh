#!/bin/bash
#
# Install OpenMPI.
#
# Too long to compile, so I chose to just archive a pre-built fixed 
# installation instead (while keeping everything for a from-scratch
# build handy just in case).


# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# OpenMPI needs some prerequisites, make sure they are installed
yum install -y gcc gcc-c++

# And untar preinstalled tarball anchoring at / (note the '-C' option).
tar -C $ROOTDEST --preserve -zxf $SCRIPTS/openmpi/openmpi-1.10.2_gcc-4.8.5_installed.tar.gz
