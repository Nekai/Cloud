#!/bin/bash

source ./000_config.sh
TORQUEPATH=$SCRIPTS/torque

#Install all of torque's dependencies
yum -y install libtool openssl-devel libxml2-devel boost-devel gcc gcc-c++

#Export the needed path
export PATH=/usr/local/bin/:/usr/local/sbin/:$PATH


#Stop firewalld
#systemctl stop firewalld
#systemctl mask firewalld

#Install and enable iptables
#yum -y install iptables-services #Install iptables
#systemctl enable iptables #enable iptables 
#service iptables start && service iptables save # start the service
#iptables-save > /tmp/iptables.mod #Create an editable iptable

#This spot for setting up IPTABLES (if it breaks)

$TORQUEPATH/torque-package-clients-linux-x86_64.sh
$TORQUEPATH/torque-package-devel-linux-x86_64.sh
$TORQUEPATH/torque-package-docs-linux-x86_64.sh
$TORQUEPATH/torque-package-server-linux-x86_64.sh
$TORQUEPATH/torque-package-mom-linux-x86_64.sh


#Configure and start trqauthd
cp $TORQUEPATH/trqauthd /etc/init.d
chkconfig --add trqauthd
echo /usr/local/lib > /etc/ld.so.conf.d/torque.conf
#ldconfig
systemctl start trqauthd.service

echo $hostname > /var/spool/torque/server_name # Set servername as hostname

# Configure and start pbs_server and pbs_mom
cp $TORQUEPATH/pbs_server /etc/init.d
chkconfig --add pbs_server
echo /usr/local/lib > /etc/ld.so.conf.d/torque.conf
#ldconfig
service pbs_servive restart

cp $TORQUEPATH/pbs_mom /etc/init.d
echo /usr/local/lib > /etc/ld.so.conf.d/torque.conf
chkconfig --add pbs_mom
#ldconfig
service pbs_mom restart


