#!/bin/bash

source $(dirname "$BASH_SOURCE")/../../000_config.sh
BLCRPATH=$FILES/blcr

#Export the needed path
export PATH=/usr/local/bin/:/usr/local/sbin/:$PATH


#untar blcr

tar -xzvf blcr-0.8.6_b4.tar.gz

cd blcr-0.8.6_b4

mkdir builddir

cd builddir

../configure

make


make install


cd $FILES/torque

./022_auto_torque_host.sh
