CloudFS
=======

Filesystem which allows to treat any http-accessible file as local.

Dependencies
------------

g++
libfuse-dev

Compiling
---------

Just run make.

Usage
-----

Mount:

    ./cloudfs directory

Add some files:

    ln -s URL directory/filename

Have fun!

    cat directory/foo.txt
    mplayer directory/bar.mp3

Features
--------

* You can add files without remounting
* You don't need anything special to add files (just ln)
* Sufficient to play 720p videos on good connections
* Files are seekable (in most cases)

Limitations (for now)
---------------------

* Non-optimal implementation (new connection is made for every read)
* No support for directories
* Connections can't be redirected by server
* Files can't be deleted
* Missing HTTPS support
* Doesn't time-out (if application tries to read and server is not responding, application hangs)
* Everything is owned by root and public (444)
* ln complains about Input/output error. (this is not problem, just little bit annoying)
* Poorly documented

Author
------

Martin Habovštiak
License: MIT
