Introduction
=====
BigObjects - Petascale Object support for gluster, s3 and other distributed storage systems.

Architecture
=====

## Diagram
<center>
<picture>
    <source src=https://github.com/BigObjects/BigObjects/raw/master/BigObjects-Architecture.webp type=image/webp >
    <source src=https://github.com/BigObjects/BigObjects/raw/master/BigObjects-Architecture.png type=image/png >
    <img src="https://github.com/BigObjects/BigObjects/raw/master/BigObjects-Architecture.png" alt="BigObjects Architecture Diagram">
</picture>
</center>

BigObject Format
=====

Command-line Interface
=====

Get/Put
~~~
$ bosh get [options]
$ bosh put [options]
~~~

Migrate from one storage driver another in parallel
~~~
$ bosh --hosts <hostfile> migrate <old_storage_driver> <new_storage_driver>
~~~

Parallel Get/Put
~~~
$ bosh --parallel get [options] <FILENAME>
$ bosh --parallel put [options] <FILENAME>
~~~

Usage:
~~~
$ bosh --help
 Usage: bosh  [options] [args]

    Commands:
      get - use the -o [--output]
         option to redirect the output to a specific file or if "-" then
        just redirect to stdout.
      put - -m or --modified is an option that will only put files
        that have changed since the last put. -C  or --chunk-size
        will upload the files in segments no larger than size.
        (see --help for more).

    Examples:
      bosh --parallel put
      bosh --parallel migrate hdfs:// gluster://

    Options:
      --version       show program's version number and exit
      -h, --help      show this help message and exit
      -p, --parallel  Parallelize
      --debug         Show debug information
      -q, --quiet     Suppress output
      -v, --verbose   Provide verbose output

~~~

BigObject API
=====
~~~
bigobject_put(**)
bigobject_get(**)
bigobject_delete(**)
~~~

Storage Drivers
=====
1. GlusterFS - gluster.so
2. s3 - s3.so

Security - TODO
=====
- data-protection/checksum
- certificates/authentication

Management and Monitoring - TODO
=====
- Measurement interface
- Profiling
