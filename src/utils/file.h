#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

namespace utils {

/**
 * @brief Create a path in the branch (if it does not already exists).
 *
 * @param branch     The branch where to create the path.
 * @param filename   The filename whose directory must be created.
 * @param mode       The mode of the created directories.
 *
 * @return 0 if succeeded, -errno from mkdir otherwise
 */
int createPath (const std::string &branch, const std::string &filename, mode_t mode);

}

#endif // FILE_UTILS_H
