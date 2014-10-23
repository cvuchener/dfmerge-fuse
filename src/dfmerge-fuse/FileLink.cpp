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

#include "FileLink.h"

#include "FileFactory.h"

FileLink::FileLink (const std::string &path, const std::string &target, bool readonly):
	File (path, "link"), _target_file (FileFactory::file_factory.newFile (target)), _readonly (readonly) {

}

FileLink::~FileLink () {
	delete _target_file;
}

int FileLink::getattr (struct stat *stbuf) const {
	return _target_file->getattr (stbuf);
}

int FileLink::mkdir (mode_t mode) {
	if (_readonly)
		return -EACCES;
	return _target_file->mkdir (mode);
}

int FileLink::unlink () {
	if (_readonly)
		return -EACCES;
	return _target_file->unlink ();
}

int FileLink::rmdir () {
	if (_readonly)
		return -EACCES;
	return _target_file->rmdir ();
}

int FileLink::rename (const char *new_name) {
	if (_readonly)
		return -EACCES;
	return _target_file->rename (new_name);
}

int FileLink::chmod (mode_t mode) {
	if (_readonly)
		return -EACCES;
	return _target_file->chmod (mode);
}

int FileLink::truncate (off_t length) {
	if (_readonly)
		return -EACCES;
	return _target_file->truncate (length);
}

int FileLink::open (int flags) {
	return _target_file->open (flags);
}

int FileLink::read (char *buffer, size_t size, off_t offset) {
	return _target_file->read (buffer, size, offset);
}

int FileLink::write (const char *buffer, size_t size, off_t offset) {
	if (_readonly)
		return -EACCES;
	return _target_file->write (buffer, size, offset);
}

int FileLink::flush () {
	if (_readonly)
		return -EACCES;
	return _target_file->flush ();
}

int FileLink::release (int flags) {
	return _target_file->release (flags);
}

int FileLink::readdir (std::vector<std::string> &dir_content) const {
	return _target_file->readdir (dir_content);
}

int FileLink::create (mode_t mode) {
	if (_readonly)
		return -EACCES;
	return _target_file->create (mode);
}
