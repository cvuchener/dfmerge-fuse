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

#ifndef FILE_LINK_H
#define FILE_LINK_H

#include "File.h"

class FileLink: public File {
public:
	FileLink (const std::string &path, const std::string &target, bool ignore_modifications = false);
	virtual ~FileLink ();

	// FUSE Operations
	virtual int getattr (struct stat *stbuf) const;
	virtual int mkdir (mode_t mode);
	virtual int unlink ();
	virtual int rmdir ();
	virtual int rename (const char *new_name);
	virtual int chmod (mode_t);
	virtual int truncate (off_t length);
	virtual int open (int flags);
	virtual int read (char *buffer, size_t size, off_t offset);
	virtual int write (const char *buffer, size_t size, off_t offset);
	virtual int flush ();
	virtual int release (int flags);
	//virtual int fsync (int datasync);
	virtual int readdir (std::vector<std::string> &dir_content) const;
	virtual int create (mode_t mode);

private:
	File *_target_file;
	bool _ignore_modifications;
};

#endif // FILE_LINK_H
