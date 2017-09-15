#!/bin/bash
#
# Populate users' authorized_keys, if any.
#
# Place them in $FILES/<username>/*.pub, and the script would append them 
# to /home/<username>/.ssh/authorized_keys

# Pull in config settings... not really needed for this script, but ordnung!
source ../files/init/config.sh

# Loop over all directories in $FILES/keys/
for dir in $FILES/keys/*; do
	# Are you a direcrory?
	[[ ! -d $dir ]] && continue

	# Directories are presumably named after users
	user=${dir##*/}

	# Dude, do you exist?
	homedir=/home/$user 		# Crude and wrong, but works for us
	[[ ! -d $homedir ]] && continue

	# Make user a .ssh directory if necessary
	sshdir=$homedir/.ssh
	if [[ ! -d $sshdir ]]; then
		echo "${0}: Creating $sshdir for $user"
		mkdir -p $sshdir
		chown $user.users $sshdir
	fi

	# OK, do we have any keys in $FILES/keys/<user> ?
	for key in $dir/* ; do
 		# We expect keys in files
		[[ ! -f $key ]] && continue
		echo "${0}: Installing $key into $sshdir"
		cat $key >> $sshdir/authorized_keys
	done
done