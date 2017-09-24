#!/bin/bash -l
#
# On PBSPro, building with '--with-tm=.....' could throw an error.
# if it does, grab a script from
# http://community.pbspro.org/t/compile-openmpi-with-pbspro-14-1-10/159/5
# and put it somewhere in the $PATH

#------------------------
os=cent7
app=openmpi
version=1.10.2
shortver=1.10.2
modules=gcc/4.8.5 		# comma-separated
tarball=$app-$version.tar.gz
prefix=/apps/$os/$app/$version
topdir=$(dirname $prefix)
#------------------------

# Make the install dir and chown it to me (for Phil's procedure to work)
test -d $prefix || mkdir -p $prefix || sudo mkdir -p $prefix
test -w $prefix || sudo chown $USER $prefix

# And call Phil's magic
module purge
# module load cuda
`pwd`/buildopenmpi prefix=$topdir gcc=$modules `pwd`/$tarball install notrial shared --with-tm=/opt/pbs "$@"

