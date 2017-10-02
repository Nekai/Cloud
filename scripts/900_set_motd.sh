#!/bin/bash
#
# Little funny tweak of original MotD (replace Figletized "Cycle Cloud"
# with "Raider Cloud").

# Pull in config settings... not really needed for this script, but ordnung!
source $(dirname "$BASH_SOURCE")/000_config.sh

head="
 ____          _      _                 ____  _                    _ 
|  _ \   __ _ (_)  __| |  ___  _ __    / ___|| |  ___   _   _   __| |
| |_) | / _\` || | / _\` | / _ \| '__|  | |    | | / _ \ | | | | / _\` |
|  _ < | (_| || || (_| ||  __/| |     | |___ | || (_) || |_| || (_| |
|_| \_\ \__,_||_| \__,_| \___||_|      \____||_| \___/  \__,_| \__,_|
"

# Original motd starts with Cycle Cloud logo, then balnk line, then useful
# stuff.  So cut until blank line (God help us if they insert one earlier!)
tail=$(sed -e '1,/^$/ d' /etc/motd) 	# Crude!

# Show off and assemble the motd.
{ echo "$head"; echo "$tail"; } | tee /etc/motd
