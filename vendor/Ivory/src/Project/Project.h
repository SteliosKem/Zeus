#pragma once
#include <string>
#include <filesystem>
#include <memory>

namespace Ivory {
	struct ProjectSettings {
		std::string name = "Untitled Project";

		std::filesystem::path assets_directory;
		std::filesystem::path default_scene;
	};

	class Project {
	public:
		Project() {}

		static const std::filesystem::path& get_assets_dir() { return s_active_project->m_settings.assets_directory; }
		static void set_active_project(const std::shared_ptr<Project>& project) { s_active_project = project; }
		static std::shared_ptr<Project> get_active_project() { return s_active_project; }
		static ProjectSettings* get_settings() { return &s_active_project->m_settings; }

		static std::shared_ptr<Project> create();
		static std::shared_ptr<Project> load(const std::filesystem::path& path);
		static bool save_active(const std::filesystem::path& path);
	private:
		ProjectSettings m_settings;

		inline static std::shared_ptr<Project> s_active_project;
	};
}