#include "SceneHierarchy.h"
#include "imgui.h"
#include "Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui_internal.h"
#include <filesystem>
#include "Project/Project.h"
#include "ImGui/ImGuiNotify.h"
#include "UI.h"

namespace Ivory {
	SceneHierarchy::SceneHierarchy(const std::shared_ptr<Scene>& scene) {
		set_context(scene);
		
	}
	void SceneHierarchy::set_context(const std::shared_ptr<Scene>& scene) {
		m_context = scene;
		m_selection_context = {};
	}
	void SceneHierarchy::draw_vec3_internal(Entity entity, const std::string& label, glm::vec3& values, float speed, float reset_value, float column_width) {
		Quantity quant;
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		if (label == "Velocity")
			add_to_graph(entity, "Velocity", Quantity::Velocity);
		else if (label == "Acceleration")
			add_to_graph(entity, "Acceleration", Quantity::Acceleration);
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
		if(label == "Translation")
			add_to_graph(entity, "Horizontal Position", Quantity::HorizontalPosition);
		else if (label == "Velocity")
			add_to_graph(entity, "Horizontal Velocity", Quantity::HorizontalVelocity);
		else if (label == "Acceleration")
			add_to_graph(entity, "Horizontal Acceleration", Quantity::HorizontalAcceleration);
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
		if (label == "Translation")
			add_to_graph(entity, "Vertical Position", Quantity::VerticalPosition);
		else if (label == "Velocity")
			add_to_graph(entity, "Vertical Velocity", Quantity::VerticalVelocity);
		else if (label == "Acceleration")
			add_to_graph(entity, "Vertical Acceleration", Quantity::VerticalAcceleration);
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

	void SceneHierarchy::add_to_graph(Entity entity, const std::string& quantity, Quantity quantity_type) {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GRAPH")) {
				std::string name = entity.get_component<TagComponent>().tag + " " + quantity;
				m_grapher->add(entity, name, quantity_type);
			}
			ImGui::EndDragDropTarget();
		}
	}

	template<typename T, typename UIFunction>
	static void draw_component(const std::string& name, Entity entity, UIFunction ui_func) {
		static const float padding = 5.0f;
		if (entity.has_component<T>()) {
			ImVec2 cursor = ImGui::GetCursorPos();
			cursor.y += padding;
			ImGui::SetCursorPos(cursor);
			auto& component = entity.get_component<T>();
			ui_func(component);
		}
	}

	void SceneHierarchy::on_imgui_render(bool is_playing, bool show_inspector, bool show_hierarchy) {
		m_is_playing = is_playing;
		if (show_hierarchy) {
			ImGui::Begin("Scene Hierarchy");
			//if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			//	m_selection_context = {};
			auto view = m_context->m_registry.view<TagComponent>();
			int i = 0;
			for (auto entity : view) {
				draw_entity_node({ entity, m_context.get() }, i % 2);
				i++;
			}

			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
				//if (ImGui::MenuItem("Create Entity"))
				//	m_context->create_entity();
				if (ImGui::MenuItem("Add Point Mass")) {
					Entity point_mass = m_context->create_entity("New Point Mass");
					point_mass.add_component<SpriteRendererComponent>();
					point_mass.add_component<PointMassComponent>();
					set_selected(point_mass);
				}
				if (ImGui::MenuItem("Add Spring")) {
					Entity spring = m_context->create_entity("New Spring", true);
					spring.add_component<SpringComponent>();
					set_selected(spring);
				}
				if (ImGui::MenuItem("Add Cable")) {
					Entity cable = m_context->create_entity("New Cable", true);
					cable.add_component<CableComponent>();
					set_selected(cable);
				}if (ImGui::MenuItem("Add Rod")) {
					Entity rod = m_context->create_entity("New Rod", true);
					rod.add_component<RodComponent>();
					set_selected(rod);
				}
				ImGui::EndPopup();
			}

			ImGui::End();
		}

		if (show_inspector) {
			ImGui::Begin("Inspector");
			if (m_selection_context) {
				if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
					if (ImGui::MenuItem("Add Component"))
						ImGui::OpenPopup("AddComponent");
					ImGui::EndPopup();
				}
				draw_components(m_selection_context);


			}
			else {
				ImVec2 cursor = ImGui::GetCursorPos();
				cursor.y += 5.0f;
				ImGui::SetCursorPos(cursor);
				ImGui::Text("Select a scene entity to inspect its elements");
			}
			ImGui::End();
		}
	}

	void SceneHierarchy::draw_entity_node(Entity entity, bool even) {
		ImGui::PushID((void*)(uint32_t)entity);
		auto& tag = entity.get_component<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_selection_context == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		static constexpr float button_size = 20;
		bool to_set_context = false;

		ImGui::AlignTextToFramePadding();
		//bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.tag.c_str());
		bool opened = false;
		hierarchy_item(tag.tag.c_str(), flags, ImGui::GetContentRegionAvail().x - button_size, m_selection_context == entity, even);
		if (ImGui::IsItemClicked()) {
			m_selection_context = entity;
		}
		bool deleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity")) {
				if (!m_is_playing)
					deleted = true;
				else
					ImGui::InsertNotification({ ImGuiToastType::Info, 3000, "Cannot delete entities while in runtime!" });
			}
			if (ImGui::MenuItem("Duplicate Entity")) {
				if (m_allowed_to_action)
					m_selection_context = m_context->copy_entity(entity);
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - button_size);
		ImGui::ImageButton((ImTextureID)m_knot_icon->get_rendererID(), ImVec2{ button_size, button_size }, ImVec2{ 0,1 }, ImVec2{ 1,0 }, 0);
		if (ImGui::BeginDragDropSource()) {
			ImGui::BeginTooltip();
			ImGui::Text(tag.tag.c_str());
			ImGui::EndTooltip();
			ImGui::SetDragDropPayload("HIERARCHY_ITEM", &entity.get_component<IdComponent>().id, sizeof(Uuid*) + 8, ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}
		
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop");

		

		

		if (opened) {
			ImGui::TreePop();
		}

		if (deleted) {
			m_context->destroy_entity(entity);
			if (m_selection_context == entity)
				m_selection_context = {};
		}
		ImGui::PopID();
	}


	void SceneHierarchy::draw_components(Entity entity) {
		const ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen;

		if (entity.has_component<TagComponent>()) {
			auto& tag = entity.get_component<TagComponent>();
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.tag.c_str());
			ImVec2 cursor = ImGui::GetCursorPos();
			cursor.y += 5.0f;
			ImGui::SetCursorPos(cursor);
			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
				tag.tag = std::string(buffer);
			}
		}

		/*if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Sprite Renderer")) {
				m_selection_context.add_component<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Circle Renderer")) {
				m_selection_context.add_component<CircleRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Point Mass")) {
				m_selection_context.add_component<PointMassComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Spring")) {
				m_selection_context.add_component<SpringComponent>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}*/

		draw_component<TransformComponent>("Transform Component", entity, [entity, this](auto& component) {
			float speed = 0.1f;

			//glm::vec2 translation{component.translation.x, component.translation.y};
			draw_vec3_internal(entity, "Translation", component.translation, speed / 2);
			//component.translation = { translation, 0.0f };


			glm::vec3 rotation = glm::degrees(component.rotation);
			draw_vec3_control("Rotation", rotation, speed / 2);
			component.rotation = glm::radians(rotation);
			draw_vec3_control("Scale", component.scale, speed / 5, 1.0f);
		});

		draw_component<SpriteRendererComponent>("Sprite Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			//ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});

		draw_component<CircleRendererComponent>("Circle Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			});

		draw_component<PointMassComponent>("Point Mass Component", entity, [entity, this](auto& component) {
			float mass = component.point_mass.get_mass();
			draw_label("Mass", mass, 0.05f);
			component.point_mass.set_mass(mass);
			draw_label("Restitution", component.point_mass.get_restitution(), 0.05f, "The factor which determines the bounciness of the object", 1.0f);
			draw_label("Static Friction", component.point_mass.get_static_friction_factor(), 0.05f, "Static Friction Factor (WIP)", 0.0f);
			draw_label("Dynamic Friction", component.point_mass.get_dynamic_friction_factor(), 0.05f, "Dynamic Friction Factor (WIP)", 0.0f);

			bool is_static = component.point_mass.is_static();
			draw_checkbox("Is Static", &is_static, "Determines if the object will move at all throughout the simulation");
			component.point_mass.set_static(is_static);

			bool only_plain_collision = component.point_mass.does_only_collide_plainly();
			draw_checkbox("Collide plainly", &only_plain_collision, "Determines if a collision with this object will only affect position");
			component.point_mass.set_only_collide_plainly(only_plain_collision);
			draw_checkbox("Affected by Gravity", &component.affected_by_gravity);

			//ImGui::DragFloat2("Position", glm::value_ptr(component.point_mass.get_position()), 0.1f);
			glm::vec3 vel = { component.point_mass.get_velocity() , 0 };
			draw_vec3_internal(entity, "Velocity", vel, 0.1/5);
			component.point_mass.set_velocity({ vel.x, vel.y });
			glm::vec3 acc = { component.point_mass.get_acceleration() , 0 };
			draw_vec3_internal(entity, "Acceleration", acc, 0.1 / 5);
			component.point_mass.set_acceleration({ acc.x, acc.y });
			draw_label("Damping", component.point_mass.get_damping(), 0.0025f, "Velocity will be multiplied by this factor every frame, simulating drag forces");

			draw_checkbox("Ignore Collisions", &component.ignore_collisions, "Determines if the object will take part in collisions");
			draw_checkbox("Is Circle", &component.is_circle, "Used for circle collisions and correct visual feedback");

			if (ImGui::Button("Add Force")) {
				component.forces_info["Force " + std::to_string(component.force_counter++)];
			}

			for (auto& [name, force] : component.forces_info) {
				glm::vec3 force_vec = { component.forces_info[name].force_vector, 0 };
				draw_vec3_control(name.c_str(), force_vec, 0.1 / 5, 1.0f);
				component.forces_info[name].force_vector = { force_vec.x, force_vec.y };
			}
		});

		draw_component<SpringComponent>("Spring Component", entity, [this](auto& component) {
			draw_label("Rest Length", component.spring.get_rest_length(), 0.1f, "The default length of the spring", 0.0f);
			draw_label("Spring Constant", component.spring.get_constant(), 0.1f, "The hardness value of the spring", 0.0f);

			draw_checkbox("Force on Both Sides", &component.spring.get_first_object_affected(), "If this is set to true, the spring will excert force only on the second attached object, making the first one acting as an anchor");
			ImGui::Text("Attached Object 1");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if(component.first_object_id)
				ImGui::Button(m_context->get_by_uuid(component.first_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.first_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("X##1"))
				component.first_object_id = 0;
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.second_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			
			if (ImGui::Button("X##2"))
				component.second_object_id = 0;
			});

		draw_component<CableComponent>("Cable Component", entity, [this](auto& component) {
			draw_label("Max Length", component.cable.get_max_length(), 0.1f, "The Maximum length the cable can achieve before tugging non-static objects", 0.0f);

			ImGui::Text("Attached Object 1");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.first_object_id)
				ImGui::Button(m_context->get_by_uuid(component.first_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.first_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("X##1"))
				component.first_object_id = 0;
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.second_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("X##2"))
				component.second_object_id = 0;
			});

		draw_component<RodComponent>("Rod Component", entity, [this](auto& component) {
			draw_label("Length", component.rod.get_length(), 0.1f, "The length of the solid rod", 0.0f);

			ImGui::Text("Attached Object 1");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.first_object_id)
				ImGui::Button(m_context->get_by_uuid(component.first_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.first_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("X##1"))
				component.first_object_id = 0;
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					component.second_object_id = *(Uuid*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			if (ImGui::Button("X##2"))
				component.second_object_id = 0;
			});

		draw_component<GravityComponent>("Gravity Component", entity, [](auto& component) {
			ImGui::DragFloat2("Intensity", glm::value_ptr(component.gravity.get_gravity()), 0.0025f);
			});
	}

	void SceneHierarchy::set_selected(Entity entity) {
		m_selection_context = entity;
	}
}