#pragma once
#include "Project.h"

namespace Ivory {
	class ProjectSerializer {
	public:
		ProjectSerializer(std::shared_ptr<Project> project);

		bool serialize(const std::filesystem::path& path);
		bool deserialize(const std::filesystem::path& path);
	private:
		std::shared_ptr<Project> m_project;
	};
}