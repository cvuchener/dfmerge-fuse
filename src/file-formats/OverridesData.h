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

#ifndef OVERRIDES_DATA_H
#define OVERRIDES_DATA_H

#include "TokenParser.h"

#include <string>
#include <map>
#include <vector>

/**
 * @brief This class implements the data model for twbt overrides.txt
 */
class OverridesData: protected TokenParser {
public:
	OverridesData ();

	void read (std::istream &stream);
	void write (std::ostream &stream);
	void merge (const OverridesData &other);
	void diff (const OverridesData &other);

protected:
	virtual void processToken (const std::vector<std::string> &values);

private:
	struct tileset_t {
		std::string font, fullfont;
	};
	std::map<std::string, tileset_t> _tilesets;
	std::vector<std::vector<std::string>> _overrides;
};

#endif // OVERRIDES_DATA_H
