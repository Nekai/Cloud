# Lmod installation

Installing Lmod is quick, so could potentially be done from scratch.
But in the interest of simplicity and speed of node initialization, 
I chose to just archive a pre-built fixed installation instead
(while keeping everything for a from-scratch build handy just in case).

In this directory:
  - 'lmod-7.6.8_installed.tar.gz' is a tarball of a complete Lmod-7.6.8
    installation (same one as on Raider).  
    Untar as root with 'tar -C / --preserve -zxf lmod-7.6.8_installed.tar.gz'

  - 'build_lmod.sh' is a complete script to clone from Github and build 
    the latest and greatest Lmod.

  - 'RCAC_addons' are tweaks from RCAC (necessary for fresh installations).
    'build_lmod.sh' uses them, 'lmod-7.6.8_installed.tar.gz' has them 
    incorporated already (of course).
