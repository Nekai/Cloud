#!/bin/bash
# Let's fix the sshd_config file to allow for password authentication :)


# First source the config
source 000_config.sh


# Our tweaks to SSHD configuration. Appending rather than overwriting.
[ -d $ROOTDEST/etc/ssh/ ] || mkdir -p $ROOTDEST/etc/ssh/
cat >>$ROOTDEST/etc/ssh/sshd_config <<-EOF
	# Make sure we allow password authentication
	PasswordAuthentication yes
EOF


#Eventually we will need to restart sshd service after running this on production level
#service sshd restart
