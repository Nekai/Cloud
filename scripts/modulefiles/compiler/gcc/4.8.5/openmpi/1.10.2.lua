
family("mpi")

local app = "openmpi"
local version = "1.10.2"
local contact = "lev"
local license_type = "bsd"
local modroot = "/apps/cent7/openmpi/1.10.2/gcc-4.8.5"

-- Automatically build hierarchy path
local hierA  = hierarchyA(app .. "/" .. version, 1)
local mpath = "/opt/modulefiles/mpi/" .. hierA[1] .. "/" .. app .. "/" .. version
if isDir(mpath) then
    prepend_path("MODULEPATH", mpath)
end
if isloaded("trial") then
    local trialmpath = "/opt/modulefiles/trial/mpi/" .. hierA[1] .. "/" .. app .. "/" .. version
    if isDir(trialmpath) then
	prepend_path("MODULEPATH", trialmpath)
    end
end

prepend_path("PATH",modroot.."/bin")
prepend_path("LD_LIBRARY_PATH",modroot.."/lib")

setenv("MPI_HOME",modroot)
setenv("CC","mpicc")
setenv("CXX","mpiCC")
setenv("F77","mpif77")
setenv("FC","mpif90")

