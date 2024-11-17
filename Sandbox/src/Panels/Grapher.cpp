#include "Grapher.h"
#include "ImGui/ImPlot/implot.h"
#include "Scene/Components.h"

namespace Ivory {
	void Grapher::on_imgui_render(bool has_recording, int32_t time) {
		ImGui::Begin("Grapher");
		if (m_map.empty())
			ImGui::Text("Drag and Drop the Rope to a Quantity to graph its recorded changes");
		ImGui::ImageButton((ImTextureID)m_knot_icon->get_rendererID(), ImVec2{ 20, 20 }, ImVec2{ 0,1 }, ImVec2{ 1,0 }, 0);
		if (ImGui::BeginDragDropSource()) {
			ImGui::BeginTooltip();
			ImGui::Text("Graph");
			ImGui::EndTooltip();
			ImGui::SetDragDropPayload("GRAPH", nullptr, 0, ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop");
		ImGui::SameLine();
		if (ImGui::Button("Clear Items"))
			reset();

		if (ImGui::CollapsingHeader("Recording Quantities")) {
			for (auto& [name, list] : m_map) {
				ImGui::Text(name.c_str());
				ImGui::SameLine();
				if (ImGui::Button(std::string("X##" + name).c_str()))
					remove(name);
			}
		}

		if(has_recording)
			if (ImPlot::BeginPlot("Plot", {ImGui::GetContentRegionAvail()})) {
				int32_t times[1] = {time};
				ImPlot::PlotInfLines("Time", times, 1);
				for (auto& [name, list] : m_map) {
					ImPlot::PlotLine(name.c_str(), &list[0], list.size());
				}
				ImPlot::EndPlot();
			}
		ImGui::End();
	}

	void Grapher::insert(const std::string& name, float value) {
		m_map[name].push_back(value);
	}

	void Grapher::add(Entity entity, const std::string& name, Quantity quantity) {
		m_map[name] = std::vector<float>();
		m_watch_list[name] = WatchInfo{ entity, quantity };
	}

	void Grapher::remove(const std::string& name) {
		m_map.erase(name);
		m_watch_list.erase(name);
	}

	void Grapher::reset() {
		m_map.clear();
		m_watch_list.clear();
	}

	void Grapher::clear_lists() {
		for (auto& [key, val] : m_map)
			val.clear();
	}
	
	void Grapher::update() {
		for (auto& [name, watch_info] : m_watch_list) {
			Alchemist::PointMass2D* pm = &m_scene->get_registry().get<PointMassComponent>((entt::entity)watch_info.entity).point_mass;
			float value = 0;
			switch (watch_info.quantity) {
			case Quantity::HorizontalPosition:
				value = pm->get_position().x;
				break;
			case Quantity::VerticalPosition:
				value = pm->get_position().y;
				break;
			case Quantity::Velocity:
				value = glm::length(pm->get_velocity());
				break;
			case Quantity::HorizontalVelocity:
				value = pm->get_velocity().x;
				break;
			case Quantity::VerticalVelocity:
				value = pm->get_velocity().y;
				break;
			case Quantity::Acceleration:
				value = glm::length(pm->get_acceleration());
				break;
			case Quantity::HorizontalAcceleration:
				value = pm->get_acceleration().x;
				break;
			case Quantity::VerticalAcceleration:
				value = pm->get_acceleration().y;
				break;
			}
			insert(name, value);
		}
	}
}