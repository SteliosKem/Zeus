#include "pch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Ivory {
	std::shared_ptr<Project> Project::create() {
		s_active_project = std::make_shared<Project>();
		return s_active_project;
	}

	std::shared_ptr<Project> Project::load(const std::filesystem::path& path) {
		s_active_project = std::make_shared<Project>();
		ProjectSerializer serializer(s_active_project);
		if (serializer.deserialize(path)) {
			return s_active_project;
		}
		return nullptr;
	}

	bool Project::save_active(const std::filesystem::path& path) {
		ProjectSerializer serializer(s_active_project);
		return serializer.serialize(path);
	}
}