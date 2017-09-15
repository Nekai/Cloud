# Scripts

These scripts will be executed in lexicographical order as the
_root/system_ user. Scripts inside this directory can be used to customize
each node in the cluster, for example by adding users, creating
directories, copying data, etc. These files are downloaded to
`/mnt/cluster-init/scripts` on Linux.  Each script will be run to
completion once and only once.  Once a file has been executed
sucessfully, a file with a .run extension will be created in the
`/mnt/cluster-init/run/executables` directory.

Per cluster-init section in the docs
(https://archive.cyclecomputing.com/cyclecloud/latest/guide/cluster_init.html#exe
cutables),

> You can create subdirectories inside the executables directory,
> but files located in subdirectories will not be run. You can use
> subdirectories as a place to put library and support files for the
> scripts intended to be run, or you can put scripts that you will
> manually run under certain circumstances. This can be particularly
> useful for python cluster-init scripts, since it allows storing 
> modules relative to the script.


If the script does not run to completion, STDOUT and STDERR are stored for
later debugging in the directory `/mnt/cluster-init/log/executables` with
filenames similar to the one being run but appended with a timestamp. For
example:

```
01-add-root-key.sh.2013-10-22T14:42:10-04:00.err
01-add-root-key.sh.2013-10-22T14:42:10-04:00.out
```

By default, failed scripts **will** prevent the node from successfully
converging.  This is the correct behavior in almost all cases. (A failed
executable is defined as one that returns a non-zero exit code).
