#include "WorldSettings.h"
#include "imgui.h"
#include "UI.h"

namespace Ivory {
	void WorldSettings::on_imgui_render() {
		ImGui::Begin("World Settings");
		ImVec2 cursor = ImGui::GetCursorPos();
		cursor.y += 5.0f;
		ImGui::SetCursorPos(cursor);
		m_gravity_intensity = m_scene->get_gravity();
		draw_label("Gravity Intensity", m_gravity_intensity, 0.1f);
		draw_label("Time Factor", m_scene->get_time_factor(), 0.05f);
		m_scene->set_gravity(m_gravity_intensity);
		ImGui::End();
	}
}