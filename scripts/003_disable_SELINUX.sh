#!/bin/bash

source $(dirname "$BASH_SOURCE")/000_config.sh

#Disable until next reboot
setenforce 0

#disable it permanently
#sed -i 's/enforcing/disabled' /etc/sysconfig/selinux
sed -i -e '/^SELINUX=/ s/.*/SELINUX=disabled/' /etc/sysconfig/selinux #thanks lev
sed -i -e '/^SELINUX=/ s/.*/SELINUX=disabled/' /etc/selinux/config #here too for good measure


