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

DFDirs::DFDirs (const std::string &base_game, const std::string &user_dir):
	_base_game (base_game), _user_dir (user_dir) {
}

void DFDirs::addModDir (const std::string &mod_dir) {
	_mod_dirs.push_front (mod_dir);
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
