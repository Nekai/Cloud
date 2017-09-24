#!/bin/bash
#
# Install packages we know we'll need

# Pull in config settings... not really needed for this script, but ordnung!
source $(dirname "$BASH_SOURCE")/000_config.sh

# Where the list is. 
PKGLIST=$SCRIPTS/packages/packagelist.txt

# And go!
# List is space- or newline-separated, with everything after any '#' 
# considered a comment and ignored.
echo "$BASH_SOURCE: First run"
yum install -y $(sed -e 's/#.*//' $PKGLIST)
echo

# And once again for good measure
echo "$BASH_SOURCE: Second run"
yum install -y $(sed -e 's/#.*//' $PKGLIST)
echo
