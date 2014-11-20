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

#include <cstdio>
#include <cstdlib>

extern "C" {
#include <fuse.h>
#include <getopt.h>
}

#include "DFMergeFuse.h"
#include "DFDirs.h"
#include "../utils/Log.h"

static void print_usage (char *prog_name) {
	fprintf (stderr,
	         "Usage: %s [options] <mount_point> [mod ...]\n"
	         "Options are:\n"
	         "  -b|--basedir <game_dir>      Set the base game directory\n"
	         "  -u|--userdir <user_dir>      Set the user files directory\n"
	         "  -m|--moddir <mod_dir>        Add a mod search directory\n"
	         "  -L|--log <filename>          Log output in <filename>\n",
	         prog_name);
}

int main (int argc, char *argv[]) {
	enum {
		BASE_DIR_OPT = 256,
		USER_DIR_OPT,
		MOD_DIR_OPT,
		LOG_OPT,
	};
	struct option longopts[] = {
		{ "basedir", required_argument, nullptr, BASE_DIR_OPT },
		{ "userdir", required_argument, nullptr, USER_DIR_OPT },
		{ "moddir", required_argument, nullptr, MOD_DIR_OPT },
		{ "log", required_argument, nullptr, LOG_OPT },
	};

	int opt;
	while (-1 != (opt = getopt_long (argc, argv, "b:u:m:L:", longopts, nullptr))) {
		switch (opt) {
		case 'b':
		case BASE_DIR_OPT:
			DFDirs::df_dirs.setBaseGameDir (optarg);
			break;

		case 'u':
		case USER_DIR_OPT:
			DFDirs::df_dirs.setUserDir (optarg);
			break;

		case 'm':
		case MOD_DIR_OPT:
			DFDirs::df_dirs.addModSearchDir (optarg);
			break;

		case 'L':
		case LOG_OPT:
			Log::addLogFile (optarg);
			break;

		default:
			print_usage (argv[0]);
			return EXIT_FAILURE;
		}
	}
	if (argc-optind < 1) {
		print_usage (argv[0]);
		return EXIT_FAILURE;
	}
	char *mount_point = argv[optind];
	for (int i = optind + 1; i < argc; ++i)
		if (!DFDirs::df_dirs.addMod (argv[i])) {
			fprintf (stderr, "Could not find mod \"%s\".\n", argv[i]);
		}

	if (!DFDirs::df_dirs.hasValidBaseGameDir ()) {
		fprintf (stderr, "No valid base game directory set.\n");
		return EXIT_FAILURE;
	}
	if (!DFDirs::df_dirs.hasValidUserDir ()) {
		fprintf (stderr, "No valid user file directory set.\n");
		return EXIT_FAILURE;
	}

	DFMergeFuse fs;
	return fs.mount (mount_point);
}
