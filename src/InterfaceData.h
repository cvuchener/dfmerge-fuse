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

#ifndef INTERFACE_DATA_H
#define INTERFACE_DATA_H

#include "TokenParser.h"

#include <string>
#include <map>
#include <list>
#include <vector>

/**
 * @brief This class implements the data model for the interface.txt
 * file.
 *
 * The interface.txt is considered to be a list of BIND token with child
 * token. A BIND token has three values: '[BIND:<action>:<repeat_mode>]'.
 * BIND token are merged in a similar way to init files. Child token are
 * added to the corresponding BIND token. They can be hidden with a
 * DELETE token ('[DELETE:KEY:'a']' will hide '[KEY:'a']').
 */
class InterfaceData: protected TokenParser {
public:
	InterfaceData ();

	void read (std::istream &stream);
	void write (std::ostream &stream);
	void merge (const InterfaceData &other);
	void diff (const InterfaceData &other);

protected:
	virtual void processToken (const std::vector<std::string> &values);

private:
	struct bind_t {
		std::string repeat;
		std::list<std::vector<std::string>> tokens;
		std::list<std::vector<std::string>> delete_tokens;
	};

	std::map<std::string, bind_t> _binds;

	bind_t *_current_bind; // Used in parsing
};

#endif // INTERFACE_DATA_H
