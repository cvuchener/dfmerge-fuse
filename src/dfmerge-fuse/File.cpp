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

#include "File.h"

#include <cstdio>

extern "C" {
#include <errno.h>
}

File::File (const std::string &path, const std::string &fsname): _path (path), _fsname (fsname) {
}

File::~File () {
}

#define UNIMPLEMENTED do { \
	fprintf (stderr, "Warning: %s is not implemented in filesystem %s (%s)\n", \
		 __FUNCTION__, \
		 _fsname.c_str (), \
		 _path.c_str ()); \
	return -ENOSYS; \
	} while (0)

int File::getattr (struct stat *stbuf) const {
	UNIMPLEMENTED;
}

int File::mkdir (mode_t mode) {
	UNIMPLEMENTED;
}

int File::unlink () {
	UNIMPLEMENTED;
}

int File::rmdir () {
	UNIMPLEMENTED;
}

int File::rename (const char *new_name) {
	UNIMPLEMENTED;
}

int File::chmod (mode_t) {
	UNIMPLEMENTED;
}

int File::truncate (off_t length) {
	UNIMPLEMENTED;
}

int File::open (int flags) {
	UNIMPLEMENTED;
}

int File::read (char *buffer, size_t size, off_t offset) {
	UNIMPLEMENTED;
}

int File::write (const char *buffer, size_t size, off_t offset) {
	UNIMPLEMENTED;
}

int File::flush () {
	UNIMPLEMENTED;
}

int File::release (int flags) {
	UNIMPLEMENTED;
}

/*int File::fsync (int datasync) {
	UNIMPLEMENTED;
}*/

int File::readdir (std::vector<std::string> &dir_content) const {
	UNIMPLEMENTED;
}

int File::create (mode_t mode) {
	UNIMPLEMENTED;
}
