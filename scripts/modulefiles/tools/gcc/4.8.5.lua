
family("compiler")

whatis("This is a system-wide (default) GCC, placed here for convenience")
whatis("(and also because NVIDIA HPL and HPCG binaries may need older version).")

local version = "4.8.5"
local app = "gcc"
local contact =	"lev"
local license_type = "gpl"
local modroot = "/usr" 		-- Well, doh! ;-)

-- Automatically build hierarchy path
local mpath = "/opt/modulefiles/compiler/" .. app .. "/" .. version .. "/"
if isDir(mpath) then
        prepend_path("MODULEPATH", mpath)
end
if isloaded("trial") then
        local trialmpath = "/opt/modulefiles/trial/compiler/" .. app .. "/" .. version .. "/"
        if isDir(trialmpath) then
                prepend_path("MODULEPATH", trialmpath)
        end
end


-- These are not needed for system-wide GCC. 
-- And especially DO NOT define default PATH here (or you'll be 
-- very sorry upon unload!)
-- prepend_path("PATH",modroot.."/bin")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/isl-0.16.1/lib")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/mpfr-3.1.5/lib")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/mpc-1.0.3/lib")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/gmp-6.1.2/lib")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/lib")
-- prepend_path("LD_LIBRARY_PATH",modroot.."/lib64")

setenv("CC","gcc")
setenv("CXX","g++")
setenv("FC","gfortran")
setenv("F90","gfortran")

setenv("GCC_HOME",modroot)
setenv("LINK_LAPACK","-llapack -lblas -lgfortran")

