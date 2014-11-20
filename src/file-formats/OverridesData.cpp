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
#include "../utils/Log.h"

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
		writeToken (stream, "OVERRIDE", override.first, override.second);
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
		std::vector<std::string> translated_override (override.second);
		std::map<std::string, std::string>::const_iterator tr_it;
		if ((tr_it = tileset_translation.find (override.second[0])) != tileset_translation.end ())
			translated_override[0] = tr_it->second;
		std::map<std::vector<std::string>, std::vector<std::string>>::iterator or_it;
		if ((or_it = _overrides.find (override.first)) != _overrides.end ())
			or_it->second = translated_override;
		else
			_overrides.emplace (override.first, translated_override);
	}
}

void OverridesData::diff (const OverridesData &other) {
	Log::error << "OverridesData::diff is not implemented" << std::endl;
	// TOIMPLEMENT not used by FileMerge as overrides.txt is readonly but can be usefull for dfdiff
}

void OverridesData::processToken (const std::vector<std::string> &values) {
	if (values.size () < 1) {
		Log::warning << "Empty token in overrides data" << std::endl;
		return;
	}
	if (values[0] == "TILESET") {
		if (values.size () < 4) {
			Log::warning << "Too few values in TILESET token: ";
			writeToken (Log::warning, values);
			Log::warning << std::endl;
			return;
		}
		_tilesets.emplace (values[3], (tileset_t){ .font = values[1], .fullfont = values[2] });
	}
	else if (values[0] == "OVERRIDE") {
		std::vector<std::string> key, value;
		unsigned int tileset_index = (values[2] == "T" ? 4 : 6);
		if (tileset_index >= values.size ()) {
			Log::warning << "Too few values in OVERRIDE token: ";
			writeToken (Log::warning, values);
			Log::warning << std::endl;
			return;
		}
		for (unsigned int i = 1; i < tileset_index; ++i)
			key.push_back (values[i]);
		for (unsigned int i = tileset_index; i < values.size (); ++i)
			value.push_back (values[i]);
		std::map<std::vector<std::string>, std::vector<std::string>>::iterator it;
		if ((it = _overrides.find (key)) != _overrides.end ())
			it->second = value;
		else
			_overrides.emplace (key, value);
	}
}
