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

#ifndef TOKEN_PARSER_H
#define TOKEN_PARSER_H

#include <istream>
#include <vector>
#include <string>
#include <regex>

/**
 * @brief A token parser.
 *
 * Subclasses must implements processToken, it will be called for each
 * token.
 */
class TokenParser {
public:
	TokenParser();

	/**
	 * @brief Parse tokens.
	 *
	 * This method will call processToken for each token found.
	 *
	 * @param stream The stream to parse.
	 */
	void parse (std::istream &stream);

protected:
	virtual void processToken (const std::vector<std::string> &values) = 0;

private:
	static const std::regex TOKEN_REGEX;
};

#endif // TOKEN_PARSER_H
