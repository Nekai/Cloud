#!/bin/bash
#
# Install packages we know we'll need

# Pull in config settings... not really needed for this script, but ordnung!
source $(dirname "$BASH_SOURCE")/000_config.sh

# Add EPEL repo
yum install -y epel-release
yum repolist

# Where the list is. 
PKGLIST_FIRST=$SCRIPTS/packages/packagelist-1.txt
PKGLIST_SECOND=$SCRIPTS/packages/packagelist-2.txt

# And go!
# List is space- or newline-separated, with everything after any '#' 
# considered a comment and ignored.
echo "$BASH_SOURCE: First run"
yum install -y $(sed -e 's/#.*//' $PKGLIST_FIRST)
echo

# And once again for good measure
echo "$BASH_SOURCE: Second run"
yum install -y $(sed -e 's/#.*//' $PKGLIST_SECOND)
echo
