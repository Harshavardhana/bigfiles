Introduction
=====
BigFiles - Very-large-file support for distributed storage systems.

Architecture
=====

## Diagram
<center>
<img src="https://github.com/Harshavardhana/BigFiles/raw/master/BigFiles-Architecture.png" alt="BigFiles Architecture Diagram">
</center>

BigFile Format
=====

Command-line Interface
=====

Get/Put
~~~
$ bigfile get [options] <FILENAME>
$ bigfile put [options] <FILENAME>
~~~

Migrate from one storage driver another in parallel
~~~
$ bigfile --hosts <hostfile> migrate <old_storage_driver> <new_storage_driver>
~~~

Parallel Get/Put
~~~
$ bigfile --parallel get [options] <FILENAME>
$ bigfile --parallel put [options] <FILENAME>
~~~

Usage:
~~~
$ bigfile --help
~~~

BigFile API
=====
~~~
bigfile_fops {
...
bigfile_open(**)
bigfile_close(**)
bigfile_append(**)
...
}
~~~
Storage Driver API
=====
1. GlusterFS - gluster.so
2. Local-FS - local-fs.so (Developers/Testing)
3. HDFS - hdfs.so

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