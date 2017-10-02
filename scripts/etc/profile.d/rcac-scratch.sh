MYUID=$(id -u)
ID=$(whoami)
if [ $MYUID != 0 ]
then
	CLUSTER_SCRATCH=/shared/scratch/$ID
	export CLUSTER_SCRATCH
        export RCAC_SCRATCH=$CLUSTER_SCRATCH

	if [[ ! -d $RCAC_SCRATCH ]]; then
		mkdir -p $RCAC_SCRATCH
	fi
fi
