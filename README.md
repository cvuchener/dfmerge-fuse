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

    dfmerge-fuse [options] <mount_point> [<mod> ...]

 * `mount_point` Directory where the merged filesystem will be mounted.

    The options are:
 * `-b|--basedir <dir>` The base (vanilla) game directory. The default is search in `$XDG_DATA_DIRS`/dfmerge-fuse/df.
 * `-u|--userdir <dir>` The user files directory (must be writeable). The default is set to `$XDG_CONFIG_HOME`/dfmerge-fuse/userfiles, if it exists.
 * `-m|--moddir <dir>` Add a directory for searching mods. Default search directories are `$XDG_DATA_DIRS`/dfmerge-fuse/mods.

### Get patch file from full files

    dfdiff init|interface base_file mod_file

Print the path file on the standard output

 * `init|interface` Algorithm to use
 * `base_file` File from the base game
 * `mod_file` Full file from the mod
