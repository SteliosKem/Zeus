#include "Items.h"
#include "imgui.h"
#include "Rendering/Texture.h"

namespace Ivory {
	void add_item(const std::string& name, std::shared_ptr<Texture2D> texture, PhysicsObjectType* type) {
		bool selected = true;
		ImGui::Image((ImTextureID)texture->get_rendererID(), ImVec2{35, 35}, ImVec2{0,1}, ImVec2{1,0});
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::Selectable(name.c_str(), &selected, ImGuiSelectableFlags{}, { ImGui::GetContentRegionAvail().x, 35.0f });
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Drag and Drop to add to scene");
		if (ImGui::BeginDragDropSource()) {
			ImGui::BeginTooltip();
			ImGui::Text(name.c_str());
			ImGui::EndTooltip();
			ImGui::SetDragDropPayload("ADD_ITEM", (void*)type, sizeof(PhysicsObjectType*) + 8, ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}
		
		ImGui::PopStyleVar();
		ImVec2 cursor = ImGui::GetCursorPos();
		cursor.y += 5.0f;
		ImGui::SetCursorPos(cursor);
	}

	void Items::on_imgui_render() {
		ImGui::Begin("Items");
		ImVec2 cursor = ImGui::GetCursorPos();
		cursor.y += 5.0f;
		ImGui::SetCursorPos(cursor);
		add_item("Point Mass (Square)", m_square_tex, &m_point_mass_square);
		add_item("Point Mass (Circle)", m_circle_tex, &m_point_mass_circle);
		add_item("Floor", m_floor_tex, &m_floor);
		add_item("Wall", m_wall_tex, &m_wall);
		add_item("Spring", m_spring_tex, &m_spring);
		add_item("Cable", m_rope_tex, &m_rope);
		add_item("Rod", m_rope_tex, &m_rod);
		add_item("Anchored Spring", m_spring_tex, &m_spring);
		add_item("Anchored Cable", m_rope_tex, &m_rope);
		add_item("Anchored Rod", m_rope_tex, &m_rod);

		ImGui::End();
	}
}