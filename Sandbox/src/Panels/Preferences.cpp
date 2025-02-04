#include "Preferences.h"
#include "imgui.h"
#include "UI.h"
#include <fstream>
#include "yaml-cpp/yaml.h"

namespace Ivory {
	void Preferences::on_imgui_render() {
		ImGui::Begin("Preferences");
		draw_label("Time per Frame", m_time_per_frame, 0.001f);
		ImGui::TextWrapped("Note: This will set the amount of time (in seconds) in between each frame in the physics engine (dt). "
						   "This will only affect the Recording mode and not the plain Simulate mode. "
					       "It is recommended to set this value to as little as possible, since the lower the time between frames the more accurate the simulation will be. "
					       "If this is running on a slower/lower end machine it is advised to not set this value as low because it will take up more RAM");
		if (ImGui::Button("Save"))
			save();
		ImGui::End();
	}

	void Preferences::load() {
		std::ifstream stream("config.yaml");
		std::stringstream str_stream;
		str_stream << stream.rdbuf();

		YAML::Node data = YAML::Load(str_stream.str());
		set_time_per_frame(data["TimePerFrame"].as<float>());
	}

	void Preferences::save() {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "TimePerFrame";
		out << YAML::Value << m_time_per_frame;

		out << YAML::EndMap;

		std::ofstream fout("config.yaml");
		fout << out.c_str();
	}
}