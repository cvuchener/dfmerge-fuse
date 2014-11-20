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

#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

class Log: public std::ostream {
public:
	static Log info, debug, warning, error;

	static void addLogFile (const char *filename);

private:
	class LogBuf: public std::stringbuf {
	public:
		LogBuf (const std::string &prefix, std::ostream *out);
		virtual int sync ();

		static void addFileOutput (const char *filename);

	private:
		std::string _prefix;
		std::ostream *_out;

		static std::vector<std::ostream *> _file_out; // TODO: use std::vector<std::ostream> when gcc 5 is out and streams have a move constructor
	};

	Log (const std::string &prefix, std::ostream *out);
	virtual ~Log ();

	LogBuf _buf;
};

#endif // LOG_H
