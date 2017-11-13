#!/bin/bash

source $(dirname "$BASH_SOURCE")/../../scripts/000_config.sh
BLCRPATH=$FILES/blcr

#Export the needed path
export PATH=/usr/local/bin/:/usr/local/sbin/:$PATH


#untar blcr

tar -xzvf blcr-0.8.6_b4.tar.gz

cd blcr-0.8.6_b4
BLCRDIR=$(pwd)

mkdir $BLCRDIR/builddir

cd $BLCRDIR/builddir

$BLCRDIR/configure

make


make install


echo $(pwd)
cd $INIT

cd Callum
./auto_setup.sh 2
