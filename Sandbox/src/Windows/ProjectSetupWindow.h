#pragma once
#include <string>
#include <functional>

namespace Ivory {
	class ProjectSetupWindow {
	public:
		void on_imgui_render();
		void set_create_callback(std::function<void(const std::string&, const std::string&)> func) { m_create_callback = func; }
		void show(bool _show) { m_show = _show; }
		bool show() { return m_show; }
		std::string get_name() { return m_project_name; }
		std::string get_path() { return m_project_path; }
	private:
		enum class CurrentlySelected {
			None,
			Create,
			Open
		};
		enum class ProjectTypeSelected {
			_2D,
			_3D,
		};
		CurrentlySelected m_selected_action = CurrentlySelected::None;
		ProjectTypeSelected m_selected_project = ProjectTypeSelected::_2D;
		std::string m_project_name;
		std::string m_project_path;

		std::function<void(const std::string&, const std::string&)> m_create_callback;
		bool m_show = false;
	};
}