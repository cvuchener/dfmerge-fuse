#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>

namespace utils {
std::vector<std::string> split (const std::string &subject, char sep, bool keep_empty = true);
}

#endif // STRING_H
