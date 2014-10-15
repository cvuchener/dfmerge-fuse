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

#include "InterfaceData.h"

#include "TokenWriter.h"

#include <iostream>

static std::list<std::vector<std::string>>::iterator findToken (std::list<std::vector<std::string>> &tokens, const std::vector<std::string> &token);

InterfaceData::InterfaceData () {
}

void InterfaceData::read (std::istream &stream) {
	_binds.clear ();
	_current_bind = nullptr;
	parse (stream);
}

void InterfaceData::write (std::ostream &stream) {
	for (const auto &bind: _binds) {
		writeToken (stream, "BIND", bind.first, bind.second.repeat);
		for (const auto &token: bind.second.tokens)
			writeToken (stream, token);
		for (const auto &token: bind.second.delete_tokens)
			writeToken (stream, "DELETE", token);
	}
}

void InterfaceData::merge (const InterfaceData &other) {
	for (const auto &bind: other._binds) {
		auto my_bind = _binds.find (bind.first);
		if (my_bind == _binds.end ()) {
			_binds.emplace (bind);
		}
		else {
			(*my_bind).second.repeat = bind.second.repeat;
			auto &my_tokens = (*my_bind).second.tokens;
			auto &my_delete_tokens = (*my_bind).second.delete_tokens;
			for (const auto &token: bind.second.tokens) {
				// Remove the token from the delete list
				auto token_to_delete = findToken (my_delete_tokens, token);
				if (token_to_delete != my_delete_tokens.end ()) {
					my_delete_tokens.erase (token_to_delete);
				}
				// Add the token if it does not already exists
				if (findToken (my_tokens, token) == my_tokens.end ())
					my_tokens.push_back (token);
			}
			for (const auto &token: bind.second.delete_tokens) {
				// Remove the token
				auto token_to_delete = findToken (my_tokens, token);
				if (token_to_delete != my_tokens.end ()) {
					my_tokens.erase (token_to_delete);
				}
				// Add the token in the delete list if it is not already there
				if (findToken (my_delete_tokens, token) == my_delete_tokens.end ())
					my_delete_tokens.push_back (token);
			}
		}
	}
}

void InterfaceData::diff (const InterfaceData &other) {
	for (const auto &bind: other._binds)  {
		auto my_bind = _binds.find (bind.first);
		if (my_bind == _binds.end ()) {
			// No handling of deleted bind tokens
			continue;
		}

		auto &my_tokens = (*my_bind).second.tokens;
		auto &my_delete_tokens = (*my_bind).second.delete_tokens;
		// Diff the tokens of the bind
		for (const auto &token: bind.second.tokens) {
			auto my_token = findToken (my_tokens, token);
			if (my_token == my_tokens.end ()) {
				// The token does not exist in our data, so add it in the delete list (if not already there)
				if (findToken (my_delete_tokens, token) == my_delete_tokens.end ())
					my_delete_tokens.push_back (token);
			}
			else // Token is unchanged
				my_tokens.erase (my_token);

		}

		// Diff the tokens in the delete list
		for (const auto &token: bind.second.delete_tokens) {
			auto my_delete_token = findToken (my_delete_tokens, token);
			if (my_delete_token != my_delete_tokens.end ())
				my_delete_tokens.erase (my_delete_token);
		}

		if (my_tokens.empty () && my_delete_tokens.empty () && (*my_bind).second.repeat == bind.second.repeat) // The two binds are equal
			_binds.erase (my_bind);
	}
}

void InterfaceData::processToken (const std::vector<std::string> &values) {
	if (values[0] == "BIND") {
		if (values.size () != 3) {
			std::cerr << "Warning: Invalid BIND token: ";
			writeToken (std::cerr, values);
		}
		auto res = _binds.emplace (values[1], (struct bind_t) { .repeat =  values[2] });
		_current_bind = &(*res.first).second;
	}
	else if (values[0] == "DELETE") {
		if (!_current_bind) {
			std::cerr << "Warning: ignored DELETE token with no corresponding BIND: ";
			writeToken (std::cerr, values);
			return;
		}
		std::vector<std::string> token;
		for (unsigned int i = 1; i < values.size (); ++i)
			token.push_back (values[i]);
		_current_bind->delete_tokens.push_back (token);
	}
	else {

		if (!_current_bind) {
			std::cerr << "Warning: ignored token with no corresponding BIND: ";
			writeToken (std::cerr, values);
			return;
		}
		_current_bind->tokens.push_back (values);
	}
}

static std::list<std::vector<std::string>>::iterator findToken (std::list<std::vector<std::string>> &tokens, const std::vector<std::string> &token) {
	std::list<std::vector<std::string>>::iterator it;
	unsigned int size = token.size ();
	for (it = tokens.begin (); it != tokens.end (); ++it) {
		if ((*it).size () != size)
			continue;
		unsigned int i;
		for (i = 0; i < size; ++i)
			if ((*it)[i] != token[i])
				break;
		if (i == size) // All values were the same
			return it;
	}
	return it;
}
