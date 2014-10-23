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

#include "OverridesData.h"

#include "TokenWriter.h"

OverridesData::OverridesData () {
}

void OverridesData::read (std::istream &stream) {
	_tilesets.clear ();
	_overrides.clear ();
	parse (stream);
}

void OverridesData::write (std::ostream &stream) {
	for (const auto &tileset: _tilesets) {
		writeToken (stream, "TILESET", tileset.second.font, tileset.second.fullfont, tileset.first);
	}
	for (const auto &override: _overrides) {
		writeToken (stream, override);
	}
}

void OverridesData::merge (const OverridesData &other) {
	std::map<std::string, std::string> tileset_translation;
	int i = 0; // Counter used for generating new tileset ids

	for (const auto &tileset: other._tilesets) {
		if (_tilesets.find (tileset.first) == _tilesets.end ()) {
			tileset_translation[tileset.first] = tileset.first;
		}
		else {
			// Tileset id already exists, find another free id
			std::string id;
			while (1) {
				std::stringstream ss;
				ss << "tileset" << i;
				id = ss.str ();
				if (_tilesets.find (id) == _tilesets.end ())
					break;
				++i;
			}
			tileset_translation[tileset.first] = id;
		}
		_tilesets[tileset_translation[tileset.first]] = tileset.second;
	}

	for (const auto &override: other._overrides) {
		std::vector<std::string> translated_override (override);
		int tileset_field;
		if (override[2] == "T")
			tileset_field = 4;
		else
			tileset_field = 6;
		std::map<std::string, std::string>::const_iterator it;
		if ((it = tileset_translation.find (override[tileset_field])) != tileset_translation.end ())
			translated_override[tileset_field] = it->second;
		_overrides.push_back (translated_override);
	}
}

void OverridesData::diff (const OverridesData &other) {
	// TOIMPLEMENT not used by FileMerge as overrides.txt is readonly but can be usefull for dfdiff
}

void OverridesData::processToken (const std::vector<std::string> &values) {
	if (values[0] == "TILESET") {
		_tilesets[values[3]] = (struct tileset_t) { .font = values[1], .fullfont = values[2] };
	}
	else if (values[0] == "OVERRIDE") {
		_overrides.push_back (values);
	}
}
