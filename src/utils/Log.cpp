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

#include "Log.h"

#include <iostream>
#include <fstream>

Log Log::info ("INFO", &std::cout);
Log Log::debug ("DEBUG", &std::cerr);
Log Log::warning ("WARNING", &std::cerr);
Log Log::error ("ERROR", &std::cerr);

Log::Log (const std::string &prefix, std::ostream *out):
        std::ostream (&_buf), _buf (prefix, out) {
}

Log::~Log () {
}

void Log::addLogFile (const char *filename) {
	Log::LogBuf::addFileOutput (filename);
}

std::vector<std::ostream *> Log::LogBuf::_file_out;

Log::LogBuf::LogBuf (const std::string &prefix, std::ostream *out):
        _prefix (prefix), _out (out) {
}

int Log::LogBuf::sync () {
	*_out << "[" << _prefix << "] " << str ();
	_out->flush ();
	for (std::ostream *out: _file_out) {
		*out << "[" << _prefix << "] " << str ();
		out->flush ();
	}
	str ("");
	return 0;
}

void Log::LogBuf::addFileOutput (const char *filename) {
	_file_out.push_back (new std::ofstream (filename, std::ios::out | std::ios::trunc));
}
