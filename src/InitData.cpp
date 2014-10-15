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

#include "InitData.h"

#include "TokenWriter.h"

InitData::InitData () {
}

void InitData::read (std::istream &stream) {
	_token_map.clear ();
	parse (stream);
}

void InitData::write (std::ostream &stream) {
	for (const auto &token: _token_map) {
		writeToken (stream, token.first, token.second);
	}
}

void InitData::merge (const InitData &other) {
	for (const auto &token: other._token_map) {
		auto it = _token_map.find (token.first);
		if (it == _token_map.end ()) {
			_token_map.emplace (token);
		}
		else {
			(*it).second = token.second;
		}
	}
}

void InitData::diff (const InitData &other) {
	for (const auto &token: other._token_map) {
		auto it = _token_map.find (token.first);
		if (it == _token_map.end ()) {
			// No handling of deleted tokens
			continue;
		}
		if ((*it).second.size () != token.second.size ())
			continue;
		int nvalues = token.second.size ();
		int i;
		for (i = 0; i < nvalues; ++i)
			if ((*it).second[i] != token.second[i])
				break;
		if (i != nvalues) // A value was different
			continue;
		// Token is unchanged
		_token_map.erase (it);
	}
}

void InitData::processToken (const std::vector<std::string> &values) {
	auto it = _token_map.find (values[0]);
	if (it == _token_map.end ()) {
		auto res = _token_map.emplace (values[0], std::vector<std::string> ());
		it = res.first;
	}
	else
		(*it).second.clear ();
	for (unsigned int i = 1; i < values.size (); ++i)
		(*it).second.push_back (values[i]);
}
