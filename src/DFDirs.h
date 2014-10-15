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

#ifndef DF_DIRS_H
#define DF_DIRS_H

#include <string>
#include <list>

/**
 * @brief The DFDirs class is used to pass the direcctories to be merged
 * by the file system.
 *
 * It contains at least a base game directory and a writable directory
 * for user files. Additional mod directories can be added with
 * addModDir.
 */
class DFDirs {
public:
	/**
	 * @brief DFDirs constructor
	 *
	 * @param base_game The base game directory.
	 * @param user_dir A writable directory for user files.
	 */
	DFDirs (const std::string &base_game, const std::string &user_dir);

	/**
	 * @brief Add a mod directory.
	 *
	 * The mod is added on top of the base game and previously
	 * added mods.
	 *
	 * @param mod_dir The mod directory.
	 */
	void addModDir (const std::string &mod_dir);

	/**
	 * @brief Get the stack of directories including base game, user
	 * files and mods.
	 *
	 * The top of the stack (the user directory) is at the front.
	 *
	 * @return The list of directories.
	 */
	std::list<std::string> getDirectoryStack () const;

	/**
	 * @brief Get the writable directory for user files.
	 *
	 * @return The writable directory.
	 */
	std::string getWriteDirectory () const;

private:
	std::string _base_game;
	std::string _user_dir;
	std::list<std::string> _mod_dirs;
};

#endif // DF_DIRS_H
