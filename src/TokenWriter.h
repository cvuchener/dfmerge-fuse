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

#ifndef TOKEN_WRITER_H
#define TOKEN_WRITER_H

#include <ostream>
#include <string>
#include <vector>

void writeTokenValues (std::ostream &, bool);

template <typename... Args>
void writeTokenValues (std::ostream &stream, bool first, const std::string &value, Args... args);

template <typename... Args>
void writeTokenValues (std::ostream &stream, bool first, const std::vector<std::string> &values, Args... args);

template <typename... Args>
void writeTokenValues (std::ostream &stream, bool first, const std::string &value, Args... args) {
	if (!first)
		stream << ":";
	stream << value;
	writeTokenValues (stream, false, args...);
}

template <typename... Args>
void writeTokenValues (std::ostream &stream, bool first, const std::vector<std::string> &values, Args... args) {
	for (const auto &value: values) {
		if (first)
			first = false;
		else
			stream << ":";
		stream << value;
	}
	writeTokenValues (stream, first, args...);
}

/**
 * @brief Write the token in the stream
 *
 * @param stream The output stream.
 * @param args The values of token. They can be any number of
 * std::string or std::vector<std::string>.
 */
template <typename... Args>
void writeToken (std::ostream &stream, Args... args) {
	stream << "[";
	writeTokenValues (stream, true, args...);
	stream << "]" << std::endl;
}

#endif // TOKEN_WRITER_H
