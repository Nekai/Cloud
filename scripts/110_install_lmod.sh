#!/bin/bash
#
# Install Lmod.
#
# Installing Lmod is quick, so could potentially be done from scratch.
# But in the interest of simplicity and speed of node initialization,
# I chose to just archive a pre-built fixed installation instead
# (while keeping everything for a from-scratch build handy just in case).


# Pull in config settings
source $(dirname "$BASH_SOURCE")/000_config.sh

# Lmod needs some prerequisites, make sure they are installed
yum install -y gcc lua-devel lua-filesystem lua-posix

# And untar preinstalled tarball anchoring at / (note the '-C' option).
tar -C $ROOTDEST --preserve -zxf $SCRIPTS/lmod/lmod-7.6.8_installed.tar.gz
