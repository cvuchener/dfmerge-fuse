dfmerge-fuse
============

dfmerge-fuse is a FUSE filesystem for merging Dwarf Fortress files from several branches (vanilla game, mods, user files, ...). It uses different schemes depending on the file path:
 * Merging specific DF file format, currently:
   - Init files (init.txt, d_init.txt): unique tokens identified by their first value.
   - Interface.txt
 * Direct file access for savegames.
 * UnionFS-like for other files.

How to build
------------

dfmerge-fuse depends on FUSE (2.6 or later).

Use cmake to build:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Usage
-----

    dfmerge-fuse <mount_point> <base_game> <user_dir> [<mod_dir> ...]

 * `mount_point` Directory where the merged filesystem will be mounted.
 * `base_game` Vanilla game directory.
 * `user_dir` Directory used to store user files.
 * `mod_dir` Mod directories.
