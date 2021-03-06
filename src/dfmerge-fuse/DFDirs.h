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
 * @brief The DFDirs class is used to store the directories to be merged
 * by the file system.
 *
 * It contains at least a base game directory and a writable directory
 * for user files. Additional mod directories can be added with
 * addModDir.
 */
class DFDirs {
public:
	/**
	 * @brief Set the base game directory.
	 *
	 * @param base_game The base game directory.
	 */
	void setBaseGameDir (const std::string &base_game);

	/**
	 * @brief Set the user files directory
	 *
	 * @param user_dir A writable directory for user files.
	 */
	void setUserDir (const std::string &user_dir);

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
	 * @brief Add a directory for finding mods
	 *
	 * The new directory will be searched before previously added
	 * directories
	 *
	 * @param mod_search_dir A directory containing mods
	 */
	void addModSearchDir (const std::string &mod_search_dir);

	/**
	 * @brief Search for the mod in search directories
	 *
	 * @param mod_name The name of the mod
	 *
	 * @return true if the mod was found, false otherwise
	 */
	bool addMod (const std::string &mod_name);

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

	/**
	 * @brief Test if the current base game directory is valid
	 * @return
	 */
	bool hasValidBaseGameDir () const;

	/**
	 * @brief Test if the current user directory is valid
	 * @return
	 */
	bool hasValidUserDir () const;

	/**
	 * @brief DFDirs singleton
	 */
	static DFDirs df_dirs;

private:
	DFDirs ();

	/**
	 * @brief Test if \p path exists and is a readable directory
	 *
	 * @param path                  file to test
	 * @param test_write_access     set to true in order to also test write access
	 *
	 * @return result of the test
	 */
	static bool existsAndIsDirectory (const std::string &path, bool test_write_access = false);

	std::string _base_game;
	std::string _user_dir;
	std::list<std::string> _mod_search_dirs;
	std::list<std::string> _mod_dirs;
};

#endif // DF_DIRS_H
