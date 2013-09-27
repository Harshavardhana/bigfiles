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
$ bigobject get [options] <FILENAME>
$ bigobject put [options] <FILENAME>
~~~

Migrate from one storage driver another in parallel
~~~
$ bigobject --hosts <hostfile> migrate <old_storage_driver> <new_storage_driver>
~~~

Parallel Get/Put
~~~
$ bigobject --parallel get [options] <FILENAME>
$ bigobject --parallel put [options] <FILENAME>
~~~

Usage:
~~~
$ bigobject --help
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

Security
=====
- data-protection/checksum
- certificates/authentication

Management and Monitoring
=====
- Measurement interface
- Profiling

TODO
=====