#include "ProjectSetupWindow.h"
#include "imgui.h"
#include "Dialogs.h"

namespace Ivory {
	void ProjectSetupWindow::on_imgui_render() {
		if (!m_show)
			return;
		ImGui::SetNextWindowSize(ImVec2(700.0f, 425.0f));
		const char* window_title = ("Create Project");
		ImGui::Begin(window_title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

		ImGui::Text("Create Project");

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 200.0f);

		ImGui::BeginChild("Left Part");
		if (ImGui::Selectable("2D", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				m_selected_project = ProjectTypeSelected::_2D;
			}
		}
		if (ImGui::Selectable("3D", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				m_selected_project = ProjectTypeSelected::_3D;
			}
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		static char name_buffer[100] = "SampleProject";
		static char path_buffer[500] = "C:/";
		if (ImGui::InputText("Project Name", name_buffer, sizeof(name_buffer))) {
			m_project_name = std::string(name_buffer);
		}
		if(ImGui::InputText("Project Path", path_buffer, sizeof(path_buffer))) {
			m_project_path = std::string(path_buffer);
		}
		if (ImGui::Button("Browse")) {
			std::string str = std::string(path_buffer);
			//FileDialogs::set_open(true);
			FileDialogs::save_file("C:/", str);
			strcpy_s(path_buffer, str.c_str());
		}
		if (ImGui::Button("Create Project")) {
			m_show = false;
		}

		ImGui::End();
	}
}