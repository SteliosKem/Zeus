#pragma once
#include <string>

namespace Ivory {
	class FileDialogs {
	public:
		static void open_file(const std::string& _path, std::string& out);
		static void save_file(const std::string& _path, std::string& out);
		static bool is_open();
		static void set_open(bool set);
	};
}