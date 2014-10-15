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

#ifndef FILE_UNION_H
#define FILE_UNION_H

#include "File.h"

#include <list>

/**
 * @brief A basic UnionFS-like file system.
 *
 * The file used is the first found in the branches. If the file is not
 * in the writable branch, it will be copied on write operations.
 *
 * *_HIDDEN files are created in order to hide file when it is not
 * possible to delete them (whem unlinking or rename file in read only
 * branches).
 */
class FileUnion: public File {
public:
	FileUnion (const std::string &path, const std::list<std::string> &branches, const std::string &write_branch);
	virtual ~FileUnion ();

	virtual int getattr (struct stat *stbuf) const;
	virtual int mkdir (mode_t mode);
	virtual int unlink ();
	virtual int rmdir ();
	virtual int rename (const char *new_name);
	virtual int truncate (off_t length);
	virtual int open (struct fuse_file_info *info);
	virtual int read (char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int write (const char *buffer, size_t size, off_t offset, struct fuse_file_info *info);
	virtual int flush (struct fuse_file_info *info);
	virtual int release (struct fuse_file_info *info);
	virtual int readdir (std::vector<std::string> &dir_content) const;
	virtual int create (mode_t mode);

private:
	/**
	 * @brief Find the file in _dirs and set _real_path, _writable and _exists
	 *
	 * @return File was found
	 */
	bool findFile ();
	/**
	 * @brief Copy current file in newfilename in _write_dir
	 *
	 * @param target_name
	 */
	void copy (const std::string &newfilename) const;
	/**
	 * @brief Create a path in the writable branch (if it does not
	 * already exists.
	 *
	 * @param filename The filename whose directory must be created.
	 * @param mode The mode of the created directories.
	 */
	void createPath (const std::string &filename, mode_t mode) const;

	std::list<std::string> _branches;
	std::string _write_branch;

	std::string _real_path;
	bool _writable;
	bool _exists;
	bool _hidden;

	int _fd;
};

#endif // FILE_UNION_H
