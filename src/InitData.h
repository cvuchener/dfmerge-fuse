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

#ifndef INIT_DATA_H
#define INIT_DATA_H

#include "TokenParser.h"

#include <string>
#include <map>
#include <vector>

/**
 * @brief This class implements the data model for simple init files.
 *
 * This data model read tokens where the first value is unique like in
 * init.txt or d_init.txt. When mergin values are replaced when a token
 * with the same first value is found.
 */
class InitData: protected TokenParser {
public:
	InitData ();

	void read (std::istream &stream);
	void write (std::ostream &stream);
	void merge (const InitData &other);
	void diff (const InitData &other);

protected:
	virtual void processToken (const std::vector<std::string> &values);

private:
	std::map<std::string, std::vector<std::string>> _token_map;
};

#endif // INIT_DATA_H
