#include "Dialogs.h"
#include "ImGui/L2DFileDialog.h"

namespace Ivory {
	void FileDialogs::open_file(const std::string& _path, std::string& out) {

		static char path[500];
		if (_path.empty()) {

			strcpy_s(path, _path.c_str());
			FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, path, sizeof(path), FileDialog::FileDialogType::OpenFile);
			out = path;
		}
	}
	void FileDialogs::save_file(const std::string& _path, std::string& out) {
		static char path[500];
		if (_path.empty()) {

			strcpy_s(path, _path.c_str());
			FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, path, sizeof(path), FileDialog::FileDialogType::SelectFolder);
			out = path;
		}
	}
	bool FileDialogs::is_open() {
		return FileDialog::file_dialog_open;
	}
	void FileDialogs::set_open(bool set) {
		FileDialog::file_dialog_open = set;
	}
}