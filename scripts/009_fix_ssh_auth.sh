#!/bin/bash
# Let's fix the sshd_config file to allow for password authentication :)


# First source the config
source 000_config.sh


# Our edited sshd_config
[ -d $ROOTDEST/etc/ssh/ ] || mkdir -p $ROOTDEST/etc/ssh/
cp $FILES/etc/ssh/sshd_config $ROOTDEST/etc/ssh/


#Eventually we will need to restart sshd service after running this on production level
#sudo service sshd restart
