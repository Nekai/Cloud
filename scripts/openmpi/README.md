# OpenMPI installation

Installing OpenMPI is too long to compile, so I chose to just archive a
pre-built fixed installation instead (while keeping everything for a
from-scratch build handy just in case).

In this directory:
  - 'openmpi-1.10.2_gcc-4.8.5_installed.tar.gz' is a tarball of a complete 
    installation of OpeMPI-1.10.2 build with GCC 4.8.5.
    Untar as root with 
	'tar -C / --preserve -zxf openmpi-1.10.2_gcc-4.8.5_installed.tar.gz'

  - 'buildopenmpi' is a magical script by Dr. Phil Cheeseman to actually 
    build OpenMPI from a tarball.

  - 'buildopenmpi-1.10.2_gcc-4.8.5.sh' is a simple wrapper to the main 
    'buildopenmpi' for one specific version.


# To make a packaged version for installation
  - spin up a node with the same OS and same INIT (obviously).
  - download desired OpenMPI version tarball.
  - do the installation.
  - Use 
	'tar --preserve -zcf openmpi-X.Y.Z_compiler-P.Q.R_installed.tar.gz /apps/cent7/openmpi/X.Y.Z/compiler-P.Q.R/'
    to package (key is using '--preserve').
