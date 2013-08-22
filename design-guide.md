Introduction
=====
BigFiles - provides very-large-file support across distributed storage systems.

Architecture
=====

## Diagram
<center>
<img src="https://github.com/Harshavardhana/BigFiles/raw/master/BigFiles-Architecture.png" height="480" width="640" alt="BigFiles Architecture Diagram">
</center>

BigFile Format
=====

Command-line Interface
=====

Upload/Download file
~~~
$ bigfile upload [options] <large_file>
$ bigfile download [options] <large_file_name>
~~~

Migrate from one storage adapter another in parallel
~~~
$ bigfile --hosts <hostfile> migrate <old_storage_adapter> <new_storage_adapter>
~~~

Parallel Upload/Download file
~~~
$ bigfile --parallel upload [options] <large_file>
$ bigfile --parallel download [options] <large_file_name>
~~~

Usage:
~~~
$ bigfile --help
~~~

BigFile API
=====

bigfile_fops {
...
bigfile_open(**)
bigfile_close(**)
bigfile_append(**)
...
}

Storage Adapter API
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
