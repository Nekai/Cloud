#!/bin/bash
#
# Populate users' authorized_keys, if any.
#
# Place them in $SCRIPTS/<username>/*.pub, and the script would append them 
# to /home/<username>/.ssh/authorized_keys

# Pull in config settings... not really needed for this script, but ordnung!
source $(dirname "$BASH_SOURCE")/000_config.sh

# Loop over all directories in $FILES/keys/
for dir in $SCRIPTS/keys/*; do
	# Are you a direcrory?
	[[ ! -d $dir ]] && continue

	# Directories are presumably named after users
	user=${dir##*/}

	# Dude, do you exist?
	homedir=$(getent passwd $user | cut -f6 -d:)
	[[ ! -d $homedir ]] && continue

	# Make user a .ssh directory if necessary
	sshdir=$homedir/.ssh
	if [[ ! -d $sshdir ]]; then
		echo "${0}: Creating $sshdir for $user"
		sudo -u $user mkdir -p $sshdir
	fi

	# If the user already *has* an authorized_keys file, this means
	# that we are on a slave node and are hitting a shared homedir 
	# that has previously been createed on a master (and already had
	# everything copied back then).  So just skip it to avoid duplication.
	if [[ -e $sshdir/authorized_keys ]]; then
		echo "${0}: Skipping existing $sshdir/authorized_keys for $user"
		continue
	fi

	# OK, do we have any keys in $FILES/keys/<user> ?
	for key in $dir/* ; do
 		# We expect keys in files
		[[ ! -f $key ]] && continue
		echo "${0}: Installing $key into $sshdir"
		# Touch first (if file doesn't exist, ensures ownership by user)
#DEBUG		sudo -u $user touch $sshdir/authorized_keys
#DEBUG		cat $key >> $sshdir/authorized_keys
	done
done
