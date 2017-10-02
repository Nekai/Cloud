#!/bin/bash -l
#----------------------
app=lmod
prefix=/opt
srcdir=Lmod
tweakdir=RCAC_addons
#----------------------

# Git the code
if [[ ! -d $srcdir ]]; then
	git clone https://github.com/TACC/Lmod $srcdir
fi
cd $srcdir
version=$(git describe --tags)
# git pull

# Configure and install.
./configure --prefix=$prefix --with-module-root-path=/$prefix/modulefiles || exit 1
sudo make install || exit 1

# And RCAC tweaks to make it workable
# NB: Lmod installs itself in $prefix/lmod/$version, and makes a symlink
#     to it called $prefix/lmod/$version (so can address as needed).
instdir=$prefix/$app/$version 	# /opt/lmod/7.6.8
fixdir=$prefix/$app/$app 	# /opt/lmod/lmod -> /opt/lmod/7.6.8
sudo mkdir -pv $prefix/modulefiles/{core,tools,compiler,mpi}
sudo install ../$tweakdir/optinlmod.{sh,csh}  $instdir/init
# Force the symlink!
sudo ln -sf $fixdir/init/optinlmod.sh  /etc/profile.d/modules.sh
sudo ln -sf $fixdir/init/optinlmod.csh /etc/profile.d/modules.csh
sudo cp -a $instdir/libexec/SitePackage.lua $instdir/libexec/SitePackage.lua.orig
sudo install ../$tweakdir/SitePackage.lua  $instdir/libexec
sudo install ../verifymodules.sh $instdir/tools
