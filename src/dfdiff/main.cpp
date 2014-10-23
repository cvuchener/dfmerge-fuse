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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <string>

#include "../file-formats/InitData.h"
#include "../file-formats/InterfaceData.h"

template<class DataModel>
void printDiff (std::istream &base, std::istream &mod, std::ostream &out) {
	DataModel old_data, new_data;
	old_data.read (base);
	new_data.read (mod);
	new_data.diff (old_data);
	new_data.write (out);
}

template void printDiff<InitData> (std::istream &, std::istream &, std::ostream &);

int main (int argc, char *argv[]) {
	std::map<std::string, void (*) (std::istream &, std::istream &, std::ostream &)> data_models;
	data_models["init"] = &printDiff<InitData>;
	data_models["interface"] = &printDiff<InterfaceData>;

	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " ";
		bool first = true;
		for (const auto &data_model: data_models) {
			if (first)
				first = false;
			else
				std::cerr << "|";
			std::cerr << data_model.first;
		}
		std::cerr << " base_file mod_file" << std::endl;
		return EXIT_FAILURE;
	}

	auto data_model = data_models.find (argv[1]);
	if (data_model == data_models.end ()) {
		std::cerr << "Unknown file type: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream base_file (argv[2]), mod_file (argv[3]);
	if (!base_file) {
		std::cerr << "Cannot open " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}
	if (!mod_file) {
		std::cerr << "Cannot open " << argv[3] << std::endl;
		return EXIT_FAILURE;
	}
	data_model->second (base_file, mod_file, std::cout);
	return EXIT_SUCCESS;
}
