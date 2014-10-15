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

#ifndef FILE_DIRECT_H
#define FILE_DIRECT_H

#include "File.h"

/**
 * @brief A direct file access scheme.
 *
 * FileDirect directly use the file in the writable branch (the user
 * file directory) and do not look at the other branched. It is useful
 * for save directories.
 */
class FileDirect: public File {
public:
	/**
	 * @brief FileDirect constructor
	 *
	 * @param path The path of the file.
	 * @param branch A writable branch directory.
	 */
	FileDirect (const std::string &path, const std::string &branch);
	virtual ~FileDirect ();

	virtual int getattr (struct stat *stbuf) const;
	virtual int mkdir (mode_t mode);
	virtual int unlink ();
	virtual int rmdir ();
	virtual int rename (const char *new_name);
	virtual int chmod (mode_t);
	virtual int truncate (off_t length);
	virtual int open (struct fuse_file_info *info);
	virtual int read (char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int write (const char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int flush (struct fuse_file_info *info);
	virtual int release (struct fuse_file_info *info);
	virtual int readdir (std::vector<std::string> &dir_content) const;
	virtual int create (mode_t mode);

private:
	std::string _branch;
	int _fd;
};

#endif // FILE_DIRECT_H
