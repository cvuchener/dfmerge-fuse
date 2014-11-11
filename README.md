dfmerge-fuse
============

dfmerge-fuse is a FUSE filesystem for merging Dwarf Fortress files from several branches (vanilla game, mods, user files, ...). It uses different schemes depending on the file path:
 * Merging specific DF file format, currently:
   - Init files (init.txt, d_init.txt): unique tokens identified by their first value.
   - interface.txt
   - TWBT overrides.txt with handling of tileset ids collisions
 * Direct file access for savegames.
 * Linking raws in save directory to the general raws
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

### Mount filesystem

    dfmerge-fuse <mount_point> <base_game> <user_dir> [<mod_dir> ...]

 * `mount_point` Directory where the merged filesystem will be mounted.
 * `base_game` Vanilla game directory.
 * `user_dir` Directory used to store user files.
 * `mod_dir` Mod directories.

### Get patch file from full files

    dfdiff init|interface base_file mod_file

Print the path file on the standard output

 * `init|interface` Algorithm to use
 * `base_file` File from the base game
 * `mod_file` Full file from the mod
