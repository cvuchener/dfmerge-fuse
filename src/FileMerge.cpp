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

#include "FileMerge.h"

#include <fstream>

#include "FileFactory.h"

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fuse.h>
}

template <class DataModel>
FileMerge<DataModel>::FileMerge (const std::string &path, const std::list<std::string> &branches, const std::string &write_branch, bool readonly):
	File (path, "merge"), _branches (branches), _write_branch (write_branch), _readonly (readonly) {
	DataModel data;
	for (auto it = _branches.rbegin (); it != _branches.rend (); ++it) {
		std::ifstream file ((*it + _path).c_str ());
		if (!file.is_open ())
			continue;
		DataModel branch_data;
		branch_data.read (file);
		data.merge (branch_data);
	}
	_internal_file.clear ();
	data.write (_internal_file);
}

template <class DataModel>
FileMerge<DataModel>::~FileMerge () {
}

template <class DataModel>
int FileMerge<DataModel>::getattr (struct stat *stbuf) const {
	stbuf->st_mode = (_readonly ? 0400 : 0600) | S_IFREG;
	stbuf->st_uid = ::getuid ();
	stbuf->st_gid = ::getgid ();
	stbuf->st_size = _internal_file.str ().size ();
	stbuf->st_nlink = 1;
	return 0;
}
template <class DataModel>
int FileMerge<DataModel>::rename (const char *new_name) {
	int ret;
	int flags = O_WRONLY | O_TRUNC;
	File *new_file = FileFactory::file_factory.newFile (new_name);
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

	_internal_file.clear ();
	_internal_file.seekg (0);
	char buffer[4096];
	off_t offset = 0;
	while (_internal_file.read (buffer, sizeof (buffer))) {
		std::streamsize r = _internal_file.gcount (), w = 0;
		while (w != r) {
			int ret = new_file->write (buffer+w, r-w, offset+w);
			if (ret < 0) {
				delete new_file;
				return ret;
			}
			w += ret;
		}
		offset += r;
	}
	if (((ret = new_file->flush ()) < 0) ||
		((ret = new_file->release (flags)) < 0)) {
		delete new_file;
		return ret;
	}
	delete new_file;
	_internal_file.str ().clear ();
	return 0;
}

template <class DataModel>
int FileMerge<DataModel>::truncate (off_t length) {
	if (_readonly)
		return -EACCES;
	_internal_file.str ().resize (length);
	saveToDisk ();
	return 0;
}

template <class DataModel>
int FileMerge<DataModel>::open (int flags) {
	if (!_readonly && flags & O_TRUNC)
		_internal_file.clear ();
	return 0;
}

template <class DataModel>
int FileMerge<DataModel>::read (char *buffer, size_t size, off_t offset) {
	_internal_file.clear ();
	_internal_file.seekg (offset);
	_internal_file.read (buffer, size);
	return _internal_file.gcount ();
}

template <class DataModel>
int FileMerge<DataModel>::write (const char *buffer, size_t size, off_t offset) {
	if (_readonly)
		return -EBADF;
	_internal_file.clear ();
	_internal_file.seekp (offset);
	_internal_file.write (buffer, size);
	return size; // Always succeed?
}

template <class DataModel>
int FileMerge<DataModel>::flush () {
	if (!_readonly)
		saveToDisk ();
	return 0;
}

template <class DataModel>
int FileMerge<DataModel>::release (int flags) {
	return 0;
}

template <class DataModel>
void FileMerge<DataModel>::saveToDisk () {
	DataModel ro_data;
	for (auto it = _branches.rbegin (); it != _branches.rend (); ++it) {
		if (*it != _write_branch) {
			std::ifstream file ((*it + _path).c_str ());
			if (!file.is_open ())
				continue;
			DataModel branch_data;
			branch_data.read (file);
			ro_data.merge (branch_data);
		}
	}

	DataModel new_data;
	_internal_file.clear ();
	_internal_file.seekg (0);
	new_data.read (_internal_file);

	new_data.diff (ro_data);
	std::ofstream write_file ((_write_branch + _path).c_str (), std::ios_base::trunc);
	new_data.write (write_file);
}

// Template instantiations
template class FileMerge<InitData>;
template class FileMerge<InterfaceData>;
