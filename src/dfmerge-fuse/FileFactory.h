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

#ifndef FILE_FACTORY_H
#define FILE_FACTORY_H

#include <string>
#include <list>
#include <functional>
#include <regex>

class File;

/**
 * @brief The FileFactory class is a singleton for instantiating File
 * object of the correct subtype depending on regex matching of the path.
 */
class FileFactory {
public:
	/**
	 * @brief Instantiate a new File for path.
	 *
	 * @param path The file path.
	 *
	 * @return The new file object.
	 */
	File *newFile (const std::string &path) const;

	/**
	 * @brief FileRules singleton
	 */
	static FileFactory file_factory;

private:
	FileFactory ();

	/**
	 * @brief A File sub-class factory
	 */
	typedef std::function<File * (const std::string &path)> factory_t;

	/**
	 * @brief The rules for instantiating new File objects.
	 *
	 * newFile use the first FileFactory whose regex was matched with the file path.
	 */
	std::list<std::pair<std::regex, factory_t>> _rules;

	/**
	 * @brief The default factory used by newFile if no regex in _rules was matched.
	 */
	factory_t _default_factory;
};

#endif // FILE_FACTORY_H
