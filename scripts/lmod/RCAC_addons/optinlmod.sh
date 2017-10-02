#! /bin/bash

if [ "`/usr/bin/whoami`" != "root" -o -n "$ROOTSAFE" ] ; then
    if [ ! -e $HOME/.nomodule ] ; then
	export LUA_PATH="/usr/share/lua/5.1/?.lua;/usr/share/lua/5.1/?/init.lua;;"
	export LUA_CPATH="/usr/lib64/lua/5.1/?.so;;"
        export LMOD_SETTARG_CMD=":"
        export LMOD_FULL_SETTARG_SUPPORT=no
	export LMOD_AVAIL_STYLE="grouped:system"
	if [[ -d "/opt/modulefiles/tools" ]]; then
	    export MODULEPATH=`/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles/core`
	    export MODULEPATH=`/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles/tools`
	else
	    export MODULEPATH=`/opt/lmod/lmod/libexec/addto --append MODULEPATH /opt/modulefiles`
	fi
        export MODULESHOME=/opt/lmod/lmod
        source /opt/lmod/lmod/init/bash
	if [[ -e "/opt/modulefiles/tools/rcac.lua" ]]; then
		if [ -z "$__Init_Default_Modules" ]; then
			export __Init_Default_Modules=1;

			## ability to predefine elsewhere the default list
			LMOD_SYSTEM_DEFAULT_MODULES=${LMOD_SYSTEM_DEFAULT_MODULES:-"rcac"}
			export LMOD_SYSTEM_DEFAULT_MODULES
			module --initial_load restore
		else
			module refresh
		fi 
	fi
    fi
fi
