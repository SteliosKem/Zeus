#include "pch.h"
#include "ProjectSerializer.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Ivory {
	ProjectSerializer::ProjectSerializer(std::shared_ptr<Project> project) : m_project(project) {}

	bool ProjectSerializer::serialize(const std::filesystem::path& path) {
		const ProjectSettings* settings = m_project->get_settings();

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << settings->name;
		out << YAML::Key << "DefaultScene" << YAML::Value << settings->default_scene.string();
		out << YAML::Key << "AssetsDir" << YAML::Value << settings->assets_directory.string();
		out << YAML::EndMap;

		out << YAML::EndSeq;
		out << YAML::EndMap;


		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::deserialize(const std::filesystem::path& path) {
		ProjectSettings* settings = m_project->get_settings();

		YAML::Node data = YAML::LoadFile(path.string());
		/*try {
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e) {
			IV_CORE_ERROR("Failed to load project file {0}\n\t{1}", path.string(), e.what());
			return false;
		}*/

		auto project_node = data["Project"];
		if (!project_node)
			return false;

		settings->name = project_node["Name"].as<std::string>();
		settings->assets_directory = project_node["AssetsDir"].as<std::string>();
		settings->default_scene = project_node["DefaultScene"].as<std::string>();
		
		return true;
	}
}