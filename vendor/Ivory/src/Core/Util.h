#pragma once
#include <string>
#include <fstream>
#include "Logging/Log.h"

std::string read_file(const std::string& file_path) {
	std::ifstream file(file_path, std::ios::in | std::ios::binary);
	std::string result = "";
	if (file) {
		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();
	}
	else {
		IV_CORE_ERROR("Could not find shader file '{0}'", file_path);
	}

	return result;
}