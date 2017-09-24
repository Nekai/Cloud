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
yum install -y $(sed -e 's/#.*//' $PKGLIST)

# And once again for good measure
yum install -y $(sed -e 's/#.*//' $PKGLIST)
