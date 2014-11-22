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

#include "DFDirs.h"

#include <cstdlib>
#include <cassert>
extern "C" {
#include <unistd.h>
#include <sys/stat.h>
}
#include "../utils/string.h"

DFDirs DFDirs::df_dirs;

#define BASE_GAME_SUFFIX	"/dfmerge-fuse/df"
#define MOD_DIR_SUFFIX		"/dfmerge-fuse/mods"
#define USERFILES_DIR_SUFFIX	"/dfmerge-fuse/userfiles"

DFDirs::DFDirs () {
	const char *home_dir = getenv ("HOME");
	const char *xdg_data_home = getenv ("XDG_DATA_HOME");
	assert (xdg_data_home || home_dir);
	const char *xdg_data_dirs = getenv ("XDG_DATA_DIRS");
	const char *xdg_config_home = getenv ("XDG_CONFIG_HOME");
	assert (xdg_config_home || home_dir);

	std::string data_home, config_home;
	if (xdg_data_home)
		data_home = xdg_data_home;
	else
		data_home = std::string (home_dir) + "/.local/share";
	if (xdg_config_home)
		config_home = xdg_config_home;
	else
		config_home = std::string (home_dir) + "/.config";
	std::vector<std::string> data_dirs;
	if (xdg_data_dirs)
		data_dirs = utils::split (xdg_data_dirs, ':', false);
	else {
		data_dirs.push_back ("/usr/local/share");
		data_dirs.push_back ("/usr/share");
	}

	if (existsAndIsDirectory (data_home + BASE_GAME_SUFFIX))
		_base_game = data_home + BASE_GAME_SUFFIX;
	else {
		for (const auto &dir: data_dirs)
			if (existsAndIsDirectory (dir + BASE_GAME_SUFFIX)) {
				_base_game = dir + BASE_GAME_SUFFIX;
				break;
			}
	}

	if (existsAndIsDirectory (data_home + MOD_DIR_SUFFIX))
		_mod_search_dirs.push_back (data_home + MOD_DIR_SUFFIX);
	for (const auto &dir: data_dirs)
		if (existsAndIsDirectory (dir + MOD_DIR_SUFFIX))
			_mod_search_dirs.push_back (dir + MOD_DIR_SUFFIX);

	if (existsAndIsDirectory (config_home + USERFILES_DIR_SUFFIX, true))
		_user_dir = config_home + USERFILES_DIR_SUFFIX;
}


void DFDirs::setBaseGameDir (const std::string &base_game) {
	_base_game = base_game;
}

void DFDirs::setUserDir (const std::string &user_dir) {
	_user_dir = user_dir;
}


void DFDirs::addModDir (const std::string &mod_dir) {
	_mod_dirs.push_front (mod_dir);
}

void DFDirs::addModSearchDir (const std::string &mod_search_dir) {
	_mod_search_dirs.push_front (mod_search_dir);
}

bool DFDirs::addMod (const std::string &mod_name) {
	for (const auto &dir: _mod_search_dirs) {
		if (existsAndIsDirectory (dir + "/" + mod_name)) {
			_mod_dirs.push_front (dir + "/" + mod_name);
			return true;
		}
	}
	return false;
}

std::list<std::string> DFDirs::getDirectoryStack () const {
	std::list<std::string> dir_list = _mod_dirs;
	dir_list.push_back (_base_game);
	dir_list.push_front (_user_dir);
	return dir_list;
}

std::string DFDirs::getWriteDirectory () const {
	return _user_dir;
}

bool DFDirs::hasValidBaseGameDir () const {
	return existsAndIsDirectory (_base_game);
}

bool DFDirs::hasValidUserDir () const {
	return existsAndIsDirectory (_user_dir, true);
}

bool DFDirs::existsAndIsDirectory (const std::string &path, bool test_write_access) {
	struct stat statbuf;

	if (-1 == ::stat (path.c_str (), &statbuf))
		return false;

	if (!S_ISDIR (statbuf.st_mode))
		return false;

	return 0 == ::access (path.c_str (), R_OK | (test_write_access ? W_OK : 0));
}
