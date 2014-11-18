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

#include "TokenParser.h"

#include "../utils/string.h"

const std::regex TokenParser::TOKEN_REGEX ("^[ \t]*\\[([^\\]]*)\\]");

TokenParser::TokenParser () {
}

void TokenParser::parse (std::istream &stream) {
	std::string line;
	while (std::getline (stream, line)) {
		std::smatch results;
		if (std::regex_search (line, results, TOKEN_REGEX)) {
			std::string token = results.str (1);
			processToken (utils::split (token, ':'));
		}
	}
}
