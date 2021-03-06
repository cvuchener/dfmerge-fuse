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

#include "FileUnion.h"

#include <iostream>
#include <regex>
#include <set>

#include "FileFactory.h"

extern "C" {
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fuse.h>
}

#include "../utils/Log.h"
#include "../utils/file.h"

#define HIDDEN_SUFFIX	"_HIDDEN"

FileUnion::FileUnion (const std::string &path, const std::list<std::string> &branches, const std::string &write_branch):
	File (path, "union"), _branches (branches), _write_branch (write_branch)
{
	findFile ();
}

FileUnion::~FileUnion () {
}

bool FileUnion::findFile () {
	_hidden = false;
	for (const std::string &dirname: _branches) {
		std::string real_path = dirname + _path;
		struct stat statbuf;

		// Check for HIDDEN file
		if (-1 == ::stat ((real_path + HIDDEN_SUFFIX).c_str (), &statbuf)) {
			if (errno != ENOENT)
				Log::error << "stat on " << (real_path + HIDDEN_SUFFIX) << ": " << strerror (errno) << std::endl;
		}
		else {
			if (dirname == _write_branch) {
				_real_path = real_path;
				_writable = true;
				_exists = false;
				_hidden = true;
				return false;
			}
			else {
				// The file is hidden in a read only branch, do has if it does not exists at all
				break;
			}
		}

		// Check for the real file
		if (-1 == ::stat (real_path.c_str (), &statbuf)) {
			if (errno != ENOENT)
				Log::error << "stat on " << real_path << ": " << strerror (errno) << std::endl;
		}
		else {
			_real_path = real_path;
			_writable = (dirname == _write_branch);
			_exists = true;
			return true;
		}
	}
	_real_path = _write_branch + _path;
	_writable = true;
	_exists = false;
	return false;
}

int FileUnion::copy (const std::string &newfilename) const {
	struct stat stbuf;
	::stat (_real_path.c_str (), &stbuf);
	int src_fd = ::open (_real_path.c_str (), O_RDONLY);
	if (-1 == src_fd) {
		Log::error << "Cannot open source file " << _real_path << " for copy: " << strerror (errno) << std::endl;
		return -errno;
	}
	int ret;
	if (0 != (ret = utils::createPath (_write_branch, newfilename, 0755)))
		return ret;
	std::string target_path = _write_branch + newfilename;
	int dest_fd = ::open (target_path.c_str (), O_WRONLY | O_CREAT | O_EXCL, stbuf.st_mode);
	if (-1 == dest_fd) {
		Log::error << "Cannot open destination file " << target_path << " for copy: " << strerror (errno) << std::endl;
		::close (src_fd);
		return -errno;
	}
	char buffer[4096];
	while (1) {
		int r;
		if (-1 == (r = ::read (src_fd, buffer, sizeof (buffer)))) {
			Log::error << "Error while reading " << _real_path << " while copying: " << strerror (errno) << std::endl;
			::close (src_fd);
			::close (dest_fd);
			return -errno;
		}
		if (r == 0) // End of file
			break;
		int ret, w = 0;
		while (w != r) {
			if (-1 == (ret = ::write (dest_fd, buffer+w, r-w))) {
				Log::error << "Error while writing " << target_path << " while copying: " << strerror (errno) << std::endl;
				::close (src_fd);
				::close (dest_fd);
				return -errno;
			}
			w += ret;
		}
	}
	::close (src_fd);
	::close (dest_fd);
	return 0;
}

int FileUnion::getattr (struct stat *stbuf) const {
	struct stat real_stbuf;
	if (_hidden)
		return -ENOENT;
	if (-1 == ::stat (_real_path.c_str (), &real_stbuf))
		return -errno;
	bool executable = (real_stbuf.st_mode & S_IXUSR) ||
			  (real_stbuf.st_mode & S_IXGRP) ||
			  (real_stbuf.st_mode & S_IXOTH);
	stbuf->st_mode = (executable ? 0755 : 0644) | (real_stbuf.st_mode & S_IFMT);
	stbuf->st_uid = ::getuid ();
	stbuf->st_gid = ::getgid ();
	stbuf->st_size = real_stbuf.st_size;
	stbuf->st_nlink = 1; // TODO: Compute number of subdirectories (http://sourceforge.net/p/fuse/mailman/message/29281571/)

	return 0;
}

int FileUnion::mkdir (mode_t mode) {
	if (_exists)
		return -EEXIST;
	if (_hidden) {
		std::string hidden_path = _write_branch + _path + HIDDEN_SUFFIX;
		::unlink (hidden_path.c_str ());
		findFile ();
		// TODO: check if the file is a directory
		if (_exists)
			return 0;
	}
	int ret;
	if (0 != (ret = utils::createPath (_write_branch, _path, mode)))
		return ret;
	if (-1 == ::mkdir (_real_path.c_str (), mode))
		return -errno;
	_exists = true;
	return 0;
}

int FileUnion::unlink () {
	if (_writable) {
		if (-1 == ::unlink (_real_path.c_str ()))
			return -errno;
		_exists = false;

		if (!findFile ()) // Look for the file in the other directories
			return 0;  // The file was not found in an another directory, no need to add a _HIDDEN file
	}
	// The file is in another directory
	// Add a *_HIDDEN file to hide it
	std::string hidden_path = _write_branch + _path + HIDDEN_SUFFIX;
	int ret;
	if (0 != (ret = utils::createPath (_write_branch, _path, 0755))) {
		Log::error << "createPath failed for " << _path << ": " << strerror (-ret) << std::endl;
		return ret;
	}
	int fd;
	if (-1 == (fd = ::creat (hidden_path.c_str (), 0644))) {
		Log::error << "Cannot create hidden file " << hidden_path << ": " << strerror (errno) << std::endl;
		return -errno;
	}
	::close (fd);
	_exists = false;
	_hidden = true;
	_writable = true;
	return 0;
}

int FileUnion::rmdir () {
	if (_writable) {
		if (-1 == ::rmdir (_real_path.c_str ()))
			return -errno;
		_exists = false;

		if (!findFile ()) // Look for the file in the other directories
			return 0;  // The file was not found in an another directory, no need to add a _HIDDEN file
	}
	// The file is in another directory
	// Add a *_HIDDEN file to hide it
	std::string hidden_path = _write_branch + _path + HIDDEN_SUFFIX;
	int ret;
	if (0 != (ret = utils::createPath (_write_branch, _path, 0755))) {
		Log::error << "createPath failed for " << _path << ": " << strerror (-ret) << std::endl;
		return ret;
	}
	int fd;
	if (-1 == (fd = ::creat (hidden_path.c_str (), 0644))) {
		Log::error << "Cannot create hidden file " << hidden_path << ": " << strerror (errno) << std::endl;
		return -errno;
	}
	::close (fd);
	_exists = false;
	_hidden = true;
	_writable = true;
	return 0;
}

int FileUnion::rename (const char *new_name) {
	if (!_exists)
		return -ENOENT;
	int ret;
	File *new_file = FileFactory::file_factory.newFile (new_name);
	if (_writable && dynamic_cast<FileUnion *> (new_file)) {
		if (-1 == ::rename (_real_path.c_str (), (_write_branch + new_name).c_str ()))
			return -errno;
	}
	else {
		int flags = O_WRONLY | O_TRUNC;
		struct stat stbuf;
		if ((ret = new_file->getattr (&stbuf)) == -ENOENT) {
			if ((ret = new_file->create (0600)) < 0) {
				delete new_file;
				return ret;
			}
		}
		else if (ret < 0) {
			delete new_file;
			return ret;
		}
		else {
			if (((ret = new_file->truncate (0)) < 0) ||
				((ret = new_file->open (flags)) < 0)) {
				delete new_file;
				return ret;
			}
		}

		int src_fd = ::open (_real_path.c_str (), O_RDONLY);
		if (-1 == src_fd) {
			Log::error << "Cannot open file " << _real_path << " for renaming to a different file type: " << strerror (errno) << std::endl;
			delete new_file;
			return -errno;
		}
		char buffer[4096];
		off_t offset = 0;
		while (1) {
			int r;
			if (-1 == (r = ::read (src_fd, buffer, sizeof (buffer)))) {
				delete new_file;
				::close (src_fd);
				return -errno;
			}
			if (r == 0) // End of file
				break;
			int ret, w = 0;
			while (w != r) {
				if ((ret = new_file->write (buffer+w, r-w, offset+w)) < 0) {
					delete new_file;
					::close (src_fd);
					return ret;
				}
				w += ret;
			}
			offset += r;
		}
		::close (src_fd);
		if (((ret = new_file->flush ()) < 0) ||
			((ret = new_file->release (flags)) < 0)) {
			delete new_file;
			return ret;
		}
		delete new_file;
		if (_writable)
			::unlink (_real_path.c_str ());
	}
	if (findFile ()) {
		// Hide the files in the read only dirs
		std::string hidden_path = _write_branch + _path + HIDDEN_SUFFIX;
		::close (::creat (hidden_path.c_str (), 0644));
	}
	_path = new_name;
	_real_path = _write_branch + new_name;
	_writable = true;
	_exists = true;
	return 0;
}

int FileUnion::truncate (off_t length) {
	if (!_writable) {
		copy (_path);
		_real_path = _write_branch + _path;
		_writable = true;
	}
	if (-1 == ::truncate (_real_path.c_str (), length))
		return -errno;
	return 0;
}

int FileUnion::open (int flags) {
	_fd = ::open (_real_path.c_str (), (_writable ? flags : O_RDONLY));
	if (-1 == _fd)
		return -errno;
	return 0;
}

int FileUnion::read (char *buffer, size_t size, off_t offset) {
	if (-1 == ::lseek (_fd, offset, SEEK_SET))
		return -errno;
	int ret;
	if (-1 == (ret = ::read (_fd, buffer, size)))
		return -errno;
	return ret;
}

int FileUnion::write (const char *buffer, size_t size, off_t offset) {
	if (!_writable) { // Copy on write
		::close (_fd);
		copy (_path);
		_real_path = _write_branch + _path;
		_writable = true;
		_fd = ::open (_real_path.c_str (), O_RDWR);
	}
	if (-1 == ::lseek (_fd, offset, SEEK_SET))
		return -errno;
	int ret;
	if (-1 == (ret = ::write (_fd, buffer, size)))
		return -errno;
	return ret;
}

int FileUnion::flush () {
	return 0;
}

int FileUnion::release (int flags) {
	::close (_fd);
	return 0;
}


int FileUnion::readdir (std::vector<std::string> &dir_content) const {
	std::regex hidden_re (HIDDEN_SUFFIX "$");
	std::set<std::string> content;
	std::list<std::string>::const_reverse_iterator it;
	for (it = _branches.rbegin (); it != _branches.rend (); ++it) {
		const std::string &branch = *it;
		std::set<std::string> hidden;

		DIR *dir = ::opendir ((branch + _path).c_str ());
		if (!dir)
			continue;
		struct dirent *ent;
		while (nullptr != (ent = ::readdir (dir))) {
			std::string filename (ent->d_name);
			std::smatch results;

			if (std::regex_search (filename, results, hidden_re)) {
				// Files are hidden later, so that if both regular and HIDDEN file are presents in the same branch, it will be hidden anyway.
				hidden.insert (filename.substr (0, results.position ()));
			}
			else {
				content.insert (filename);
			}
		}
		::closedir (dir);

		for (const auto &filename: hidden)
			content.erase (filename);
	}
	dir_content.clear ();
	for (const auto &filename: content) {
		dir_content.push_back (filename);
	}
	return 0;
}


int FileUnion::create (mode_t mode) {
	if (_exists)
		return -EEXIST;
	if (_hidden) {
		std::string hidden_path = _write_branch + _path + HIDDEN_SUFFIX;
		::unlink (hidden_path.c_str ());
		_hidden = false;
	}
	int ret;
	if (0 != (ret = utils::createPath (_write_branch, _path, 0755)))
		return ret;
	_real_path = _write_branch + _path;
	if (-1 == (_fd = ::creat (_real_path.c_str (), mode)))
		return -errno;
	_exists = true;
	return 0;
}
