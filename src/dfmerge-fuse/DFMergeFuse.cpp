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

#include "DFMergeFuse.h"

#include "DFDirs.h"
#include "FileFactory.h"

#include "File.h"

#include <iostream>

#include <cstring>

extern "C" {
#include <unistd.h>
}

template <class T, typename Sig>
struct FuseOp;

template <class T, typename Ret, typename... Args>
struct FuseOp<T, Ret (*) (Args...)> {
	template <Ret (T::*method) (Args... )>
	static Ret operation (Args... args) {
		return (static_cast<T *>(fuse_get_context ()->private_data)->*method) (args...);
	}
};
#define GET_OP(T, func) &FuseOp<T, decltype(fuse_operations::func)>::operation<&T::func>

DFMergeFuse::DFMergeFuse () {
	::memset (&_fuse_ops, 0, sizeof (struct fuse_operations));
	_fuse_ops.getattr	= GET_OP (DFMergeFuse, getattr);
	_fuse_ops.mkdir		= GET_OP (DFMergeFuse, mkdir);
	_fuse_ops.unlink	= GET_OP (DFMergeFuse, unlink);
	_fuse_ops.rmdir		= GET_OP (DFMergeFuse, rmdir);
	_fuse_ops.rename	= GET_OP (DFMergeFuse, rename);
	_fuse_ops.chmod		= GET_OP (DFMergeFuse, chmod);
	_fuse_ops.truncate	= GET_OP (DFMergeFuse, truncate);
	_fuse_ops.open		= GET_OP (DFMergeFuse, open);
	_fuse_ops.read		= GET_OP (DFMergeFuse, read);
	_fuse_ops.write		= GET_OP (DFMergeFuse, write);
	_fuse_ops.flush		= GET_OP (DFMergeFuse, flush);
	_fuse_ops.release	= GET_OP (DFMergeFuse, release);
	//_fuse_ops.fsync		= GET_OP (DFMergeFuse, fsync);
	_fuse_ops.readdir	= GET_OP (DFMergeFuse, readdir);
	_fuse_ops.create	= GET_OP (DFMergeFuse, create);
	_fuse_ops.init		= GET_OP (DFMergeFuse, init);
}

int DFMergeFuse::mount (const char *mountpoint) {
	struct fuse_chan *chan = ::fuse_mount (mountpoint, nullptr);
	_fuse = ::fuse_new (chan, nullptr, &_fuse_ops, sizeof (struct fuse_operations), this);
	::fuse_set_signal_handlers (fuse_get_session (_fuse));
	return ::fuse_loop_mt (_fuse);
}

void *DFMergeFuse::init (struct fuse_conn_info *conn) {
	return this;
}

int DFMergeFuse::getattr (const char *path, struct stat *statbuf) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->getattr (statbuf);
	delete file;
	return ret;
}

int DFMergeFuse::mkdir (const char *path, mode_t mode) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->mkdir (mode);
	delete file;
	return ret;
}

int DFMergeFuse::unlink (const char *path) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->unlink ();
	delete file;
	return ret;
}

int DFMergeFuse::rmdir (const char *path) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->rmdir ();
	delete file;
	return ret;
}

int DFMergeFuse::rename (const char *path, const char *new_name) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->rename (new_name);
	delete file;
	return ret;
}

int DFMergeFuse::chmod (const char *path, mode_t mode) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->chmod (mode);
	delete file;
	return ret;
}

int DFMergeFuse::truncate (const char *path, off_t offset) {
	File *file = FileFactory::file_factory.newFile (path);
	int ret = file->truncate (offset);
	delete file;
	return ret;
}

int DFMergeFuse::open (const char *path, fuse_file_info *info) {
	File *file = FileFactory::file_factory.newFile (path);
	info->fh = _opened_files.insertFile (file);
	return file->open (info->flags);
}

int DFMergeFuse::read (const char *path, char *buffer, size_t size, off_t offset, fuse_file_info *info) {
	File *file = _opened_files.getFile (info->fh);
	return file->read (buffer, size, offset);
}

int DFMergeFuse::write (const char *path, const char *buffer, size_t size, off_t offset, fuse_file_info *info) {
	File *file = _opened_files.getFile (info->fh);
	return file->write (buffer, size, offset);
}

int DFMergeFuse::flush (const char *path, fuse_file_info *info) {
	File *file = _opened_files.getFile (info->fh);
	return file->flush ();
}

int DFMergeFuse::release (const char *path, fuse_file_info *info) {
	File *file = _opened_files.getFile (info->fh);
	int ret = file->release (info->flags);
	_opened_files.removeFile (info->fh);
	delete file;
	return ret;
}

/*int DFMergeFuse::fsync (const char *path, int datasync, fuse_file_info *info) {
	File *file = _opened_files.getFile (info->fh);
	return file->fsync (datasync);
}*/

int DFMergeFuse::readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *info) {
	File *file = FileFactory::file_factory.newFile (path);
	std::vector<std::string> dir_content;
	int ret = file->readdir (dir_content);
	if (ret == 0) {
		for (auto it = dir_content.begin (); it != dir_content.end (); ++it)
			filler (buf, (*it).c_str (), nullptr, 0);
	}
	delete file;
	return ret;
}

int DFMergeFuse::create (const char *path, mode_t mode, fuse_file_info *info) {
	File *file = FileFactory::file_factory.newFile (path);
	info->fh = _opened_files.insertFile (file);
	return file->create (mode);
}

DFMergeFuse::FileIndex::FileIndex (): _files (256, nullptr), _first_empty_index (0) {
}

uint64_t DFMergeFuse::FileIndex::insertFile (File *file) {
	uint64_t index = _first_empty_index;
	_files[_first_empty_index] = file;
	for (; _first_empty_index < _files.size (); _first_empty_index++) {
		if (_files[_first_empty_index] == nullptr) {
			return index;
		}
	}
	_files.resize (_files.size () * 2, nullptr);
	return index;
}

File *DFMergeFuse::FileIndex::getFile (uint64_t index) const {
	return _files[index];
}

void DFMergeFuse::FileIndex::removeFile (uint64_t index) {
	_files[index] = nullptr;
	if (index < _first_empty_index)
		_first_empty_index = index;
}
