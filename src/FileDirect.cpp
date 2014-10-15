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

#include "FileDirect.h"

extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fuse.h>
}

FileDirect::FileDirect (const std::string &path, const std::string &branch):
	File (path, "direct"), _branch (branch) {

}

FileDirect::~FileDirect () {
}

int FileDirect::getattr (struct stat *stbuf) const {
	if (-1 == ::stat ((_branch + _path).c_str (), stbuf))
		return -errno;
	return 0;
}

int FileDirect::mkdir (mode_t mode) {
	if (-1 == ::mkdir ((_branch + _path).c_str (), mode));
		return -errno;
	return 0;
}

int FileDirect::unlink () {
	if (-1 == ::unlink ((_branch + _path).c_str ()))
		return -errno;
	return 0;
}

int FileDirect::rmdir () {
	if (-1 == ::rmdir ((_branch + _path).c_str ()))
		return -errno;
	return 0;
}

int FileDirect::rename (const char *new_name) {
	if (-1 == ::rename ((_branch + _path).c_str (), (_branch + new_name).c_str ()))
		return -errno;
	return 0;
}

int FileDirect::chmod (mode_t mode) {
	if (-1 == ::chmod ((_branch + _path).c_str (), mode))
		return -errno;
	return 0;
}

int FileDirect::truncate (off_t length) {
	if (-1 == ::truncate ((_branch + _path).c_str (), length))
		return -errno;
	return 0;
}

int FileDirect::open (struct fuse_file_info *info) {
	if (-1 == (_fd = ::open ((_branch + _path).c_str (), info->flags)))
		return -errno;
	return 0;
}

int FileDirect::read (char *buffer, size_t size, off_t offset, struct fuse_file_info *info) {
	int r;
	if (-1 == ::lseek (_fd, offset, SEEK_SET))
		return -errno;
	if (-1 == (r = ::read (_fd, buffer, size)))
		return -errno;
	return r;
}

int FileDirect::write (const char *buffer, size_t size, off_t offset, struct fuse_file_info *info) {
	int w;
	if (-1 == ::lseek (_fd, offset, SEEK_SET))
		return -errno;
	if (-1 == (w = ::write (_fd, buffer, size)))
		return -errno;
	return w;
}

int FileDirect::flush (struct fuse_file_info *info) {
	// TODO: fsync?
	return 0;
}

int FileDirect::release (struct fuse_file_info *info) {
	if (-1 == ::close (_fd))
		return -errno;
	return 0;
}

int FileDirect::readdir (std::vector<std::string> &dir_content) const {
	DIR *dir = ::opendir ((_branch + _path).c_str ());
	if (!dir)
		return -errno;
	struct dirent *ent;
	dir_content.clear ();
	errno = 0;
	while (nullptr != (ent = ::readdir (dir))) {
		dir_content.push_back (ent->d_name);
	}
	if (errno != 0)
		return -errno;
	return 0;
}

int FileDirect::create (mode_t mode) {
	if (-1 == ::creat ((_branch + _path).c_str (), mode))
		return -errno;
	return 0;
}
