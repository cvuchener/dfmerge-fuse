/*
 * Copyright 2014 Clément Vuchener
 *
 * This file is part of dfmerge-fuse
 *
 * dfmerge-fuse is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <cstdlib>

extern "C" {
#include <fuse.h>
}

#include "DFMergeFuse.h"
#include "DFDirs.h"

int main (int argc, char *argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " mount_point base_dir user_dir [mod_dir ...]" << std::endl;
		return EXIT_FAILURE;
	}

	char *mount_point = argv[1];
	char *base_dir = argv[2];
	char *user_dir = argv[3];

	DFDirs dirs (base_dir, user_dir);
	for (int i = 4; i < argc; ++i)
		dirs.addModDir (argv[i]);

	DFMergeFuse fs (&dirs);
	return fs.mount (mount_point);
}
