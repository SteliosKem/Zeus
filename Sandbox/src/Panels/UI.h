#pragma once
#include "imgui.h"
#include <string>
#include <glm/glm.hpp>
#include <imgui_internal.h>
#include <functional>
#include "Scene/Entity.h"

namespace Ivory {
	inline void draw_label(const std::string& label, float& values, float speed, const std::string& tooltip = "", float reset_value = 0.0f, float column_width = 120.0f) {
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		if (ImGui::IsItemHovered() && !tooltip.empty()) ImGui::SetTooltip(tooltip.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0 ,0 });

		ImGui::DragFloat("##X", &values, speed);
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	inline void draw_int_label(const std::string& label, int& values, float speed, const std::string& tooltip = "", float reset_value = 0.0f, float column_width = 120.0f) {
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		if (ImGui::IsItemHovered() && !tooltip.empty()) ImGui::SetTooltip(tooltip.c_str());
		ImGui::NextColumn();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0 ,0 });

		ImGui::DragInt("##X", &values, speed);
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	inline void draw_checkbox(const std::string& label, bool* value, const std::string& tooltip = "", float column_width = 120.0f) {
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		if (ImGui::IsItemHovered() && !tooltip.empty()) ImGui::SetTooltip(tooltip.c_str());
		ImGui::NextColumn();

		ImGui::Checkbox("##C", value);

		ImGui::Columns(1);
		ImGui::PopID();
	}

	inline void draw_vec3_control(const std::string& label, glm::vec3& values, float speed, float reset_value = 0.0f, float column_width = 120.0f) {
		//ImGuiIO& io = ImGui::GetIO();
		//auto font = io.Fonts->Fonts[0];
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 button_size = { line_height + 3.0f, line_height };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.6f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.6f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.6f, 0.2f, 0.2f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 94 / 256.0f, 113 / 256.0f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0f, 94 / 256.0f, 113 / 256.0f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 94 / 256.0f, 113 / 256.0f, 1.0f });

		if (ImGui::Button("X", button_size))
			values.x = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 37 / 256.0f, 170 / 256.0f, 37 / 256.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 37 / 256.0f, 170 / 256.0f, 37 / 256.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 37 / 256.0f, 170 / 256.0f, 37 / 256.0f, 1.0f });

		if (ImGui::Button("Y", button_size))
			values.y = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.6f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.2f, 0.6f, 1.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.6f, 1.0f });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.476f, 0.39f, 0.92f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.476f, 0.39f, 0.92f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.476f, 0.39f, 0.92f, 1.0f });
		if (ImGui::Button("Z", button_size))
			values.z = reset_value;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	inline bool hierarchy_item(const char* label, ImGuiTreeNodeFlags flags = 0, float width = 0.0f, bool selected = false, bool even = false)
	{
		if (even) {
			ImVec4 col = ImGui::GetStyle().Colors[ImGuiCol_Header];
			col.x -= 0.025f;
			col.y -= 0.025f;
			col.z -= 0.025f;
			ImGui::PushStyleColor(ImGuiCol_Header, col);
		}
		if (selected)
			ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
		if (width == 0.0f)
			return ImGui::CollapsingHeader(label, flags);

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGui::SetNextItemWidth(width);
		float backup_work_max_x = window->WorkRect.Max.x;
		window->WorkRect.Max.x = window->DC.CursorPos.x + ImGui::CalcItemWidth();
		
		bool ret = ImGui::CollapsingHeader(label, flags);
		window->WorkRect.Max.x = backup_work_max_x;
		if (selected)
			ImGui::PopStyleColor();
		if (even)
			ImGui::PopStyleColor();
		return ret;
	}
}