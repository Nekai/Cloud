#!/bin/bash

source $(dirname "$BASH_SOURCE")/000_config.sh

$TORQUEPATH = $FILES/Callum

#Check for Args
if [ $# -lt 1 ]; then
  echo 1>&2 "$0: Specify how many cores to use on host compute node"
  exit 2
elif [ $# -gt 1 ]; then
  echo 1>&2 "$0: too many arguments"
  exit 2
fi

#Install all of torque's dependencies
yum -y install libtool openssl-devel libxml2-devel boost-devel gcc gcc-c++ flex byacc rpm-build

#Export the needed path
export PATH=/usr/local/bin/:/usr/local/sbin/:$PATH

tar -zxvf torque-5.1.3-1462984387_205d70d.tar.gz

cd torque-5.1.3-1462984387_205d70d

./configure

make && make install

echo '/usr/local/lib' > /etc/ld.so.conf.d/torque.conf
ldconfig 


# Configure Torque as a service
cp contrib/init.d/trqauthd /etc/init.d/
chkconfig --add trqauthd
echo /usr/local/lib > /etc/ld.so.conf.d/torque.conf
ldconfig
service trqauthd start

# Echo necessary variables
$HOST = $hostname
echo $HOST > /var/spool/torque/server_name

#Start the database, may prompt user
./torque.setup root:localhost

# Set up how many cores to use per node
$NOP = $1

echo "$pbsserver $HOST" >> /var/spool/torque/mom_priv/config
echo "$logevent 255" >> /var/spool/torque/mom_priv/config

echo "$HOST np=$NOP" >> /var/spool/torque/server_priv/nodes

cp contrib/init.d/pbs_server /etc/init.d
chkconfig --add pbs_server
service pbs_server restart

cp contrib/init.d/pbs_mom /etc/init.d
chkconfig --add pbs_mom
service pbs_mom restart


# Make Packages for compute nodes
make packages


# Restart Server
qterm
sleep 2
pbs_server
