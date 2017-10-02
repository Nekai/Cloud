--------------------------------------------------------------------------
-- This is a placeholder for site specific functions.
-- @module SitePackage

require("strict")

local hook  = require("Hook")
local dbg  = require("Dbg"):dbg()
local ModuleStack  = require("MasterControl")


--------------------------------------------------------------------------------
-- Anything in this file will automatically be loaded everytime the Lmod command
-- is run.  Here are two suggestions on how to use your SitePackage.lua file 
--
-- a) Install Lmod normally and then overwrite your SitePackage.lua file over
--    this one in the install directory.

-- b) Create a file named "SitePackage.lua" in a different directory separate
--    from the Lmod installed directory.  Then you should modify
--    your modules.sh and modules.csh (or however you initialize the "module" command)
--    with:
--
--       (for bash, zsh, etc)
--       export LMOD_PACKAGE_PATH=/path/to/the/Site/Directory
--
--       (for csh)
--       setenv LMOD_PACKAGE_PATH /path/to/the/Site/Directory
--
--    A "SitePackage.lua" in that directory will override the one in the Lmod
--    install directory.
--
-----------------------------------------------------------------------------
-- You should check to see that Lmod finds your SitePackage.lua.  If you do:
-- 
--    $ module --config
-- 
-- and it reports:
-- 
--    Modules based on Lua: Version X.Y.Z  3016-02-05 16:31
--        by Robert McLay mclay@tacc.utexas.edu
-- 
--    Description                      Value
--    -----------                      -----
--    ...
--    Site Pkg location                standard
-- 
-- Then you haven't set things up correctly.
-----------------------------------------------------------------------------
--  Any function here that is called by a module file must be registered with
--  the sandbox.  For example you have following functions in your
--  SitePackage.lua file:
--
--      function sam()
--      end
--
--      function bill()
--      end
--
--  Then you have to add any functions defined here that you wish to call inside
--  a modulefile with the sandbox by doing:
--      sandbox_registration{ sam = sam, bill = bill}

------------------------------------------------------------------------
-- DO NOT FORGET TO USE CURLY BRACES "{}" and NOT PARENS "()"!!!!
------------------------------------------------------------------------

--  As an example suppose you want to require that users of a particular package must
--  be in a special group.  You can write this function here and use it in any
--  modulefile:
--
--
--     function module_requires_group(group)
--        local grps  = capture("groups")
--        local found = false
--        for g in grps:split("[ \n]") do
--           if (g == group)  then
--              found = true
--              break
--           end
--        end
--        return found
--     end
--
--     sandbox_registration{ ['required_group'] = module_requires_group }
--
--
--  Then in any module file you can have:
--
--
--     -------------------------
--     foo/1.0.lua:
--     -------------------------
--
--     local err_message="To use this module you must be in a particular group\n" ..
--                       "Please contact foo@my.supercomputer.center to join\n"
--
--     local found = required_group("G123456")
--
--     if (not found) then
--        LmodError(err_message)
--     end
--
--     prepend_path("PATH","/path/to/Foo/Bin")
--
--  Note that here I have used the name "required_group" in the modulefile and named the
--  function as "module_requires_group".  The key is the name used in the modulefile and
--  the right is what the function is called in SitePackage.lua.  The names can be the
--  same.
--
function module_requires_group(group)
    local grps  = capture("groups")
    local found = false
    for g in grps:split("[ \n]") do
        if (g == group)  then
            found = true
            break
        end
    end
    return found
end

function log_usage(mod)
--LG--    local year=capture("date +%Y"):gsub("%s+", "")
--LG--    local logname=os.getenv("USER")
--LG--    local host=capture("hostname -s"):gsub("%s+", "")
--LG--    local jobid=os.getenv("PBS_JOBID")
--LG--    if jobid == nil then
--LG--        jobid = "nonpbs"
--LG--    end
--LG--    os.execute("logger -t modlog -p local3.info '" .. year .. " " .. logname .. " " .. host .. " " .. mod[0] .. " " .. mod[1] .. " " .. jobid .. "'") 
end

local function module_notification(module, userload, requestedload)
--LG--    -- Set path to notifications list. This can be in the lmod install, a shared filesystem, or anywherre.
--LG--    -- Gather a few other variables.
--LG--    local lmod = "/opt/lmod/notifications/"
--LG--    local notification_list = lmod .. 'notifications.list'
--LG--    local ourhostname = capture("hostname -s"):gsub("%s+", "")
--LG--
--LG--    -- Don't spam testsuite
--LG--    local queue=os.getenv("PBS_QUEUE")
--LG--    if queue == "testpbs" then
--LG--        return
--LG--    end
--LG--
--LG--    -- Workaround for module rewrites
--LG--    if requestedload == nil then
--LG--        return
--LG--    end
--LG--
--LG--    -- Open notification file list. If it does not exist, exit silently.
--LG--    local file = io.open(notification_list, "r")
--LG--    if file == nil then 
--LG--	return
--LG--    end
--LG--    local notifications = file:read("*all")
--LG--    file:close()
--LG--
--LG--    -- Gather all potential maches
--LG--    local matches = {}
--LG--    local count = 0
--LG--
--LG--    -- Match by filename
--LG--    for line in string.gmatch(notifications, "[\n^]" .. string.gsub(module.fn, "%-", '%%%-') .. "%s+[^\n]+") do
--LG--	matches[count] = line
--LG--	count = count + 1
--LG--    end
--LG--    -- Match by specific name
--LG--    for line in string.gmatch(notifications, "[\n^]" .. string.gsub(requestedload, "%-", '%%%-') .. "%s+[^\n]+") do
--LG--	matches[count] = line
--LG--	count = count + 1
--LG--    end
--LG--    -- Match by wild cart
--LG--    for line in string.gmatch(notifications, "[\n^]" .. string.gsub(module.modName, "%-", '%%%-') .. "/%*%s+[^\n]+") do
--LG--	matches[count] = line
--LG--	count = count + 1
--LG--    end
--LG--    if count > 0 then
--LG--
--LG--	-- Break down the line and hunt for match
--LG--	-- Format: module        date    daysnotice      replacement     template	hostname
--LG--	local mod, date, daysnotice, replacement, template, hostname, kill
--LG--        for i=0,count-1 do
--LG--		local line = matches[i]
--LG--		mod, date, daysnotice, replacement, template, hostname, kill = string.match(line, "([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+([^%s]+)")
--LG--		if not hostname then return end
--LG--		if string.match(ourhostname, "^" .. hostname) or hostname == '*' then
--LG--			break
--LG--		end
--LG--	end
--LG--
--LG--	-- Check validity of all our inputs
--LG--	local ok = true
--LG--	if not mod then ok = false end
--LG--	if not date then ok = false end
--LG--	if not daysnotice then ok = false end
--LG--	if not replacement then ok = false end
--LG--	if not template then ok = false end
--LG--	if not hostname then ok = false end
--LG--	if not kill then ok = false end
--LG--	
--LG--	-- Silently fail if something is missing
--LG--	if not ok then
--LG--		return
--LG--	end
--LG--
--LG--	-- Determine if we're within range of a notice, or past the notice date
--LG--	local now = capture("date +%s"):gsub("%s+", "")
--LG--	local noticedate = capture("date +%s -d '" .. date .. "'")
--LG--
--LG--	local time_delta = noticedate - now
--LG--
--LG--	local notice = nil
--LG--	if (time_delta <= 0) then
--LG--		-- Notice is past due. Display past notice.
--LG--		file = io.open(lmod .. "template." .. template .. ".past", "r")
--LG--		if file == nil then 
--LG--			return
--LG--		end
--LG--		local template = file:read("*all")
--LG--		file:close()
--LG--		notice = template
--LG--	elseif (time_delta < (daysnotice * 24 * 60 * 60)) then
--LG--		-- We're within notice range. Display message.
--LG--		file = io.open(lmod .. "template." .. template, "r")
--LG--		if file == nil then 
--LG--			return
--LG--		end
--LG--		local template = file:read("*all")
--LG--		file:close()
--LG--		notice = template
--LG--	else
--LG--		-- We're not within range yet. Exit silently.
--LG--		return
--LG--	end
--LG--
--LG--	if notice then
--LG--		-- Substite variables
--LG--		notice = string.gsub(notice, "${date}", date)
--LG--		notice = string.gsub(notice, "${mod}", requestedload)
--LG--		notice = string.gsub(notice, "${fmod}", module.modFullName)
--LG--		notice = string.gsub(notice, "${replacement}", replacement)
--LG--		local daysleft = math.floor(time_delta / 60 / 60 / 24)
--LG--		notice = string.gsub(notice, "${daysleft}", daysleft)
--LG--
--LG--		-- Print the message!
--LG--		if (time_delta <= 0) then
--LG--			-- Module is retired. Print error as to abandon ship.
--LG--			if (kill == "1") then
--LG--				LmodError(notice)
--LG--			else
--LG--				LmodMessage(notice)
--LG--			end
--LG--		else
--LG--			LmodMessage(notice)
--LG--		end
--LG--		return
--LG--	end
--LG--
--LG--    end
--LG--
end

local function load_hook(t)
    -- Called every time a module is loaded
    -- the arg t is a table:
    --     t.modFullName:  the module full name: (i.e: gcc/4.7.2)
    --     t.fn:           the file name: (i.e /apps/modulefiles/Core/gcc/4.7.2.lua)

    if (mode() ~= "load") then return end

    -- if userload is yes, the user request to load this module. Else
    -- it is getting loaded as a dependency.
    local requestedload = myModuleUsrName() 
    -- TODO: fix for lmod 7
    --local userload = (mStack:atTop()) and "yes" or "no"

    local logTbl      = {}
    logTbl[0] = t.modFullName
    logTbl[1] = t.fn

    log_usage(logTbl)
    --module_notification(t, userload, requestedload)
end

local function transition(moduleName)
--LG-- if (mode() ~= "load") then return end
--LG-- -- split long module into shorter ones, like: hdf5/1.8.11_mvapich2-1.9_intel-13.1.1.163
--LG-- local o = {}
--LG-- for k in string.gmatch(moduleName, "([%w%.%-%/]+)") do
--LG--    if (#o == 0) then
--LG--       o[#o+1] = k
--LG--    else
--LG--       o[#o+1] = k.gsub(k, "-", "/")
--LG--    end
--LG--    if string.match(k, "impi.*") and #o == 2 then
--LG--       -- if impi is there by itself, and we haven't preloaded an intel, slip in intel
--LG--       if isloaded("intel") then
--LG--          o[#o+1] = "intel"
--LG--       end
--LG--    end
--LG-- end
--LG--
--LG-- -- rewrites
--LG-- local rewrite = {}
--LG-- rewrite['binutils/2.27_gcc-4.4.7'] = "binutils"
--LG-- rewrite['cmake/3.6.0'] = "cmake"
--LG-- rewrite['git/1.8.5.1'] = "git"
--LG-- rewrite['subversion/1.9.4'] = "subversion"
--LG-- rewrite['hypre/2.11.0_impi-5.0.3.048_intel-15.0.2.164'] = "hypre/2.11.0"
--LG-- rewrite['pnetcdf/1.4.1_impi-4.1.1.036'] = "pnetcdf/1.4.1"
--LG-- if rewrite[moduleName] ~= nil then
--LG--    o[#o] = rewrite[moduleName]
--LG--    moduleName = rewrite[moduleName]
--LG-- end
--LG--
--LG-- -- user is requesting a default module (ie, no version
--LG-- -- hard cod dictionary, ugly, but easy
--LG-- local reqs = {}
--LG-- reqs['abaqus'] = {"intel/16.0.1.150"}
--LG-- reqs['abaqus/6.14-6'] = {"intel/16.0.1.150"}
--LG-- reqs['amber/14'] = {"intel/13.1.1.163", "openmpi/1.8.1"}
--LG-- reqs['atlas'] = {"gcc/4.7.2"}
--LG-- reqs['atlas/3.11.30'] = {"gcc/4.7.2"}
--LG-- reqs['binutils'] = {"utilities"}
--LG-- reqs['binutils/2.27_gcc-4.4.7'] = {"utilities"}
--LG-- reqs['cmake'] = {"utilities"}
--LG-- reqs['cmake/3.6.0'] = {"utilities"}
--LG-- reqs['espresso'] = {"intel/16.0.1.150", "impi/5.1.2.150"}
--LG-- reqs['fftw'] = {"intel/16.0.1.150", "openmpi/1.8.1"}
--LG-- reqs['git'] = {"utilities"}
--LG-- reqs['git/1.8.5.1'] = {"utilities"}
--LG-- reqs['gromacs'] = {"intel/13.1.1.163", "openmpi/1.8.1"}
--LG-- reqs['gromacs/5.0'] = {"intel/13.1.1.163", "openmpi/1.8.1"}
--LG-- reqs['hdf5'] = {"intel/16.0.1.150"}
--LG-- reqs['impi'] = {"intel/16.0.1.150"}
--LG-- reqs['impi/4.1.1.036'] = {"intel/13.1.1.163"}
--LG-- reqs['impi/5.1.1.109'] = {"intel/16.0.1.150"}
--LG-- reqs['impi/5.1.2.150'] = {"intel/16.0.1.150"}
--LG-- reqs['lammps'] = {"intel/16.0.1.150", "impi/5.1.2.150"}
--LG-- reqs['lammps/1Feb14'] = {"intel/13.1.1.163", "openmpi/1.6.3"}
--LG-- reqs['meep'] = {"intel/16.0.1.150", "impi/5.1.2.150"}
--LG-- reqs['nco'] = {"intel/13.1.1.163"}
--LG-- reqs['netcdf'] = {"intel/16.0.1.150"}
--LG-- reqs['openfoam'] = {"gcc/4.7.2"}
--LG-- reqs['openfoam/2.2.0'] = {"gcc/4.7.2"}
--LG-- reqs['openfoam/2.2.1'] = {"gcc/4.7.2"}
--LG--
--LG-- reqs['openfoam/4.1.0'] = {"gcc/5.2.0", "openmpi/1.10.1"}
--LG-- reqs['openmpi'] = {"intel/16.0.1.150"}
--LG-- reqs['paraview/5.2.0'] = {"gcc/5.2.0"}
--LG-- reqs['petsc'] = {"intel/14.0.2.144", "impi/4.1.1.036"}
--LG-- reqs['pnetcdf'] = {"intel/13.1.1.163", "impi/4.1.1.036"}
--LG-- reqs['pnetcdf/1.4.1'] = {"intel/13.1.1.163", "impi/4.1.1.036"}
--LG-- reqs['root'] = {"gcc/5.2.0"}
--LG-- reqs['root/5.34.32'] = {"gcc/5.2.0"}
--LG-- reqs['root/5.34.36'] = {"gcc/5.2.0"}
--LG-- reqs['rstudio'] = {"gcc/5.2.0"}
--LG-- reqs['subversion'] = {"utilities"}
--LG-- reqs['subversion/1.9.4'] = {"utilities"}
--LG-- reqs['vtk'] = {"intel/13.1.1.163"}
--LG-- reqs['vtk/6.0.0'] = {"intel/13.1.1.163"}
--LG--
--LG-- if reqs[moduleName] ~= nil then
--LG--    o[1] = moduleName
--LG--    for j = 1, #reqs[moduleName], 1 do
--LG--       o[2+j-1] = reqs[moduleName][j]
--LG--    end
--LG-- end
--LG--
--LG---- module names are inconsistent, figure out if one is the compiler
--LG-- if (#o > 2) then
--LG--    -- this should really look at all the base/compiler level modules but that seems hard
--LG--    -- so just look for compilers since thats mostly what we care about
--LG--    -- split this thing
--LG--    local bits = {}
--LG--    for bit in string.split(o[2], "/") do
--LG--       bits[#bits+1] = bit
--LG--    end
--LG--    if (bits[1] == "intel" or bits[1] == "gcc" or bits[1] == "pgi") then
--LG--       -- ok, we need to swap for the last one
--LG--       local temp = o[2]
--LG--       o[2] = o[#o]
--LG--       o[#o] = temp
--LG--    end
--LG-- end
--LG--
--LG-- -- Don't spam testsuite
--LG-- local queue=os.getenv("PBS_QUEUE")
--LG-- if queue == "testpbs" then
--LG--    return
--LG-- end
--LG--
--LG-- LmodMessage("***** NOTICE *****")
--LG-- LmodMessage("The software stack has been converted to hierarchical modules. Your requested module has been translated into the new hierarchy and has been loaded successfully.")
--LG-- LmodMessage("More info: https://www.rcac.purdue.edu/news/924")
--LG-- LmodMessage("Update \"module load " .. moduleName .. "\" in your job scripts to the following:\n")
--LG-- local newmod = ""
--LG-- for j = #o, 1, -1 do
--LG--
--LG--    newmod = newmod .. " " .. o[j]
--LG-- end
--LG-- LmodMessage("    module load" .. newmod)
--LG-- LmodMessage("\nUpdate job scripts ASAP - this translation will be removed on May 9th. Once your scripts are updated, execute the following command:")
--LG-- LmodMessage("    touch ~/.hierarchy")
--LG-- LmodMessage("******************")
--LG--
--LG--
--LG-- return o
--LG--	
end

local mapT =
{
   grouped = {
      ['/tools.*']         = "Tools - Load to make more modules available",
      ['/core.*']     = "Core Applications",
      ['/compiler/gcc/.*'] = "Applications built with GCC VERSION",
      ['/compiler/intel/.*'] = "Applications built with Intel VERSION",
      ['/mpi/.*/.*/openmpi/.*'] = "Applications built with OpenMPI VERSION",
      ['/mpi/.*/.*/impi/.*'] = "Applications built with Intel MPI VERSION",
      ['/mpi/.*/.*/mvapich2/.*'] = "Applications built with MVAPICH2 VERSION",
      ['/transition.*'] = "Hierarchy Transition - DO NOT USE",
      ['/utilities.*'] = "Modern versions of basic system utilities",
   },
}

local function avail_hook(t)
   dbg.print{"avail hook\n"}
   local availStyle = masterTbl().availStyle
   local styleT = mapT[availStyle]
   if (not availStyle or availStyle == "system" or styleT == nil) then
      return
   end

   for k,v in pairs(t) do
      for pat,label in pairs(styleT) do
         if (k:find(pat)) then
            if string.match(label, "VERSION") then
                local version = string.match(k, "/[^/]+$")
		version = string.gsub(version, "^/", "")
                label = string.gsub(label, "VERSION", version)
            end
	    if string.match(k, "/trial") then
		    label = "Trial " .. label 
            end
            t[k] = label
            break
         end
      end
   end
end

sandbox_registration{ ['required_group'] = module_requires_group }
sandbox_registration{ ['transition'] = transition }
hook.register("avail", avail_hook)
hook.register("load", load_hook)
