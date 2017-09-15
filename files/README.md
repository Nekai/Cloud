# Files

Files in the cluster-init `files` directory will be replicated on the
local drive of each instance. The directory provides fast local access
to its files, but data stored here will not persist when a node is
terminated.

In Linux, the cluster-init `files` directory is found at
`/mnt/cluster-init/files`. Upon start-up, each instance will sync all
files from the cloud storage scratch directory (for example
`s3://com.cyclecloud.demo.locker/clusterinit/default/files/`) to the
local `/mnt/cluster-init/files` directory.

