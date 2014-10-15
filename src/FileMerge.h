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

#ifndef FILE_MERGE_H
#define FILE_MERGE_H

#include "File.h"

#include <list>
#include <sstream>

/**
 * @brief A merge filesystem based on a data model
 *
 * FileMerge use DataModel to manage the data. The DataModel class must
 * have read, write, merge and diff methods. The diff method can do
 * nothing for read only files.
 */
template <class DataModel>
class FileMerge: public File {
public:
	/**
	 * @brief FileMerge constructor
	 *
	 * @param path The file path
	 * @param branches The list of the branches to merge (red from
	 * end to beginning)
	 * @param write_branch The writable branch where to save the
	 * difference.
	 * @param readonly A boolean indicating if the file is read
	 * only.
	 */
	FileMerge (const std::string &path, const std::list<std::string> &branches, const std::string &write_branch, bool readonly = false);
	virtual ~FileMerge ();

	virtual int getattr (struct stat *stbuf) const;
	virtual int truncate (off_t length);
	virtual int open (struct fuse_file_info *info);
	virtual int read (char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int write (const char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int flush (struct fuse_file_info *info);
	virtual int release (struct fuse_file_info *info);

private:
	void saveToDisk ();

	std::list<std::string> _branches;
	std::string _write_branch;
	bool _readonly;
	std::stringstream _internal_file;
};

// Types instantiated in FileMerge.cpp
#include "InitData.h"
#include "InterfaceData.h"
extern template class FileMerge<InitData>;
extern template class FileMerge<InterfaceData>;

#endif // FILE_MERGE_H
