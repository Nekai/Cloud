
setenv ID `whoami`

if ( $ID != "root" ) then
    setenv CLUSTER_SCRATCH /shared/scratch/$ID
    setenv RCAC_SCRATCH $CLUSTER_SCRATCH

    if ( ! -d $RCAC_SCRATCH ) then
        mkdir -p $RCAC_SCRATCH
    endif
endif
