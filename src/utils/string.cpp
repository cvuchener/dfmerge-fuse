#include "string.h"

std::vector<std::string> utils::split (const std::string &subject, char sep, bool keep_empty) {
	std::vector<std::string> elems;
	std::string::size_type pos = 0;
	std::string::size_type sep_pos;
	while (std::string::npos != (sep_pos = subject.find (sep, pos))) {
		if (keep_empty || sep-pos > 0)
			elems.push_back (subject.substr (pos, sep_pos-pos));
		pos = sep_pos+1;
	}
	elems.push_back (subject.substr (pos));
	return elems;
}
