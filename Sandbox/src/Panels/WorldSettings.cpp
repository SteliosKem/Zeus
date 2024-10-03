#include "WorldSettings.h"
#include "imgui.h"

namespace Ivory {
	void WorldSettings::on_imgui_render() {
		ImGui::Begin("World Settings");
		m_gravity_intensity = m_scene->get_gravity();
		ImGui::DragFloat("Gravity Intensity", &m_gravity_intensity, 0.1f, 0.0f, 1000000.0f);
		ImGui::DragFloat("Time Factor", &m_scene->get_time_factor(), 0.05f, 0.0f, 1000000.0f);
		m_scene->set_gravity(m_gravity_intensity);
		ImGui::End();
	}
}