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

#ifndef DF_MERGE_FUSE_H
#define DF_MERGE_FUSE_H

extern "C" {
#include <fuse.h>
}

#include <string>
#include <vector>
#include <list>
#include <functional>
#include <regex>

class File;
class DFDirs;

/**
 * @brief The DFMergeFuse class implements the FUSE filesystem.
 *
 * This filesystem use differents merging scheme (File subclasses)
 * depending on the filename.
 *
 * Because of FUSE restrictions, only one object must exist in the
 * application.
 */
class DFMergeFuse {
public:
	/**
	 * @brief DFMergeFuse constructor
	 *
	 * @param dirs The directories to use in this filesystem.
	 */
	DFMergeFuse (const DFDirs *dirs);

	/**
	 * @brief Mount the filesystem
	 *
	 * @param mountpoint The path of the mount point.
	 *
	 * @return The return value of the FUSE loop.
	 */
	int mount (const char *mountpoint);

private:
	/**
	 * @brief Instantiate a new File for path.
	 *
	 * @param path The file path.
	 *
	 * @return The new file object.
	 */
	File *newFile (const std::string &path) const;

	// FUSE Operations
	int getattr (const char *path, struct stat *statbuf);
	int mkdir (const char *path, mode_t mode);
	int unlink (const char *path);
	int rmdir (const char *path);
	int rename (const char *path, const char *new_name);
	int chmod (const char *path, mode_t mode);
	int truncate (const char *path, off_t offset);
	int open (const char *path, fuse_file_info *info);
	int read (const char *path, char *buffer, size_t size, off_t offset, fuse_file_info *info);
	int write (const char *path, const char *buffer, size_t size, off_t offset, fuse_file_info *info);
	int flush (const char *path, fuse_file_info *info);
	int release (const char *path, fuse_file_info *info);
	int fsync (const char *path, int datasync, fuse_file_info *info);
	int readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *info);
	int create (const char *path, mode_t mode, fuse_file_info *info);
	void *init (struct fuse_conn_info *conn);

	// FUSE structures
	struct fuse_operations _fuse_ops;
	struct fuse *_fuse;

	// DF Dirs
	const DFDirs *_dirs;

	// Files management
	typedef std::function<File * (const std::string &path)> FileFactory;
	/**
	 * @brief The rules for instantiating new File objects.
	 *
	 * newFile use the first FileFactory whose regex was matched with the file path.
	 */
	std::list<std::pair<std::regex, FileFactory>> _file_rules;
	/**
	 * @brief The default FileFactory used by newFile if no regex in _files_rules was matched.
	 */
	FileFactory _default_factory;

	/**
	 * @brief A file index for keeping track of opened File objects.
	 */
	class FileIndex {
	public:
		FileIndex ();

		/**
		 * @brief Insert a new file in the index.
		 * @param file The new File object.
		 * @return The file index.
		 */
		uint64_t insertFile (File *file);
		/**
		 * @brief Get an already opened file from its index.
		 * @param index The file index.
		 * @return The file object.
		 */
		File *getFile (uint64_t index) const;
		/**
		 * @brief Remove a File from the index.
		 * @param index The file index.
		 */
		void removeFile (uint64_t index);

	private:
		std::vector<File *> _files;
		uint64_t _first_empty_index;
	} _opened_files;
};

#endif // DF_MERGE_FUSE_H
