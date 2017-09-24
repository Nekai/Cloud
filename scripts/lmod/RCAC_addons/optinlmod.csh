#! /bin/csh

if ( "`/usr/bin/whoami`" != "root" ) then
    if ( ! -e ${HOME}/.nomodule ) then
	set my_hostname = `/bin/hostname -f`
	setenv LMOD_SETTARG_CMD ":"
	setenv LMOD_FULL_SETTARG_SUPPORT no
        setenv LMOD_AVAIL_STYLE "grouped:system"
        if ( -d /opt/modulefiles/tools ) then
            setenv MODULEPATH `/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles/core`
            setenv MODULEPATH `/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles/tools`
        else
            setenv MODULEPATH `/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles`
        endif
	setenv MODULESHOME /opt/lmod/lmod
	if ($?tcsh) then
		source /opt/lmod/lmod/init/tcsh
	else
		source /opt/lmod/lmod/init/csh
	endif
        if ( -e /opt/modulefiles/tools/rcac.lua ) then
		if ( ! $?__Init_Default_Modules )  then
			setenv __Init_Default_Modules 1
			if ( ! $?LMOD_SYSTEM_DEFAULT_MODULES ) then
				setenv LMOD_SYSTEM_DEFAULT_MODULES "rcac"
			endif
			module --initial_load restore
		else
			module refresh
		endif
	endif
	unset my_hostname
    endif
endif
