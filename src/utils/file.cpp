#include "file.h"

#include "string.h"
#include "Log.h"

#include <cstring>

extern "C" {
#include <sys/stat.h>
}

int utils::createPath (const std::string &branch, const std::string &filename, mode_t mode) {
	std::vector<std::string> path = utils::split (filename.substr (1), '/', false);
	path.pop_back (); // Remove the filename
	std::string current = branch;
	for (const std::string &dir: path) {
		current += "/" + dir;
		if (-1 == ::mkdir (current.c_str (), mode) && errno != EEXIST) {
			Log::error << "Cannot make directory " << current.c_str () << " in branch " << branch << ": " << strerror (errno) << "." << std::endl;
			return -errno;
		}
	}
	return 0;
}
