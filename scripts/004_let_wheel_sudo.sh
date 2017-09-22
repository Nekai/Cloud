#!/bin/bash
# Add a snippet to /etc/sudoers.d to allow members of group 'wheel' to sudo
# (apparently disabled by default on some Cyclecloud's tweaked CentOS 7 images).

# Pull in config settings
source 000_config.sh

# Our snippet
[ -d $ROOTDEST/etc/sudoers.d ] || mkdir -p $ROOTDEST/etc/sudoers.d
cat > $ROOTDEST/etc/sudoers.d/wheel2sudo <<EOF
## Apparently this is not on by default on at least some of the
## Cyclecloud's tweaked CentOS 7 images.

## Allows people in group wheel to run all commands
%wheel	ALL=(ALL)	ALL

## Same thing without a password
# %wheel	ALL=(ALL)	NOPASSWD: ALL
EOF

