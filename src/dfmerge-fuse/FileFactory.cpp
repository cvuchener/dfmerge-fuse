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

#include "DFDirs.h"
#include "FileFactory.h"

#include "FileUnion.h"
#include "FileDirect.h"
#include "FileMerge.h"
#include "FileLink.h"

#include "../utils/Log.h"
#include "../utils/file.h"

FileFactory FileFactory::file_factory;

FileFactory::FileFactory () {
	_rules.push_back (std::pair<std::regex, factory_t> (
		std::regex ("/data/save/(current|(region[0-9]+))/raw(/.*)?"),
		[] (const std::string &path) -> File * {
			std::string target = std::regex_replace (path, std::regex ("^/data/save/(current|(region[0-9]+))/raw"), "/raw");
			return new FileLink (path, target, true);
		}));

	_rules.push_back (std::pair<std::regex, factory_t> (
		std::regex ("/data/init/(init|d_init)\\.txt"),
		[] (const std::string &path) -> File * {
			return new FileMerge<InitData> (path, DFDirs::df_dirs.getDirectoryStack (), DFDirs::df_dirs.getWriteDirectory ());
		}));

	_rules.push_back (std::pair<std::regex, factory_t> (
		std::regex ("/data/init/interface\\.txt"),
		[] (const std::string &path) -> File * {
			return new FileMerge<InterfaceData> (path, DFDirs::df_dirs.getDirectoryStack (), DFDirs::df_dirs.getWriteDirectory ());
		}));

	_rules.push_back (std::pair<std::regex, factory_t> (
		std::regex ("/data/init/overrides\\.txt"),
		[] (const std::string &path) -> File * {
			return new FileMerge<OverridesData> (path, DFDirs::df_dirs.getDirectoryStack (), DFDirs::df_dirs.getWriteDirectory (), true);
		}));

	_rules.push_back (std::pair<std::regex, factory_t> (
		std::regex ("/data/save(/.*)?"),
		[] (const std::string &path) -> File * {
			std::string branch = DFDirs::df_dirs.getWriteDirectory ();
			int ret;
			if (0 != (ret = utils::createPath (branch, "/data/save", 0755))) {
				Log::error << "Cannot create the path for /data/save in the branch " << branch << ": " << ::strerror (ret) << std::endl;
			}
			return new FileDirect (path, branch);
		}));

	_default_factory = [] (const std::string &path) -> File * {
		return new FileUnion (path, DFDirs::df_dirs.getDirectoryStack (), DFDirs::df_dirs.getWriteDirectory ());
	};
}

File *FileFactory::newFile (const std::string &path) const {
	for (const auto &rule: _rules) {
		std::smatch results;
		if (std::regex_match (path, results, rule.first))
			return rule.second (path);
	}
	return _default_factory (path);
}
