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

	void SceneHierarchy::on_imgui_render() {
		ImGui::Begin("Scene Hierarchy");
		//if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		//	m_selection_context = {};
		auto view = m_context->m_registry.view<TagComponent>();
		for (auto entity : view) {
			draw_entity_node({entity, m_context.get()});
		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			//if (ImGui::MenuItem("Create Entity"))
			//	m_context->create_entity();
			if (ImGui::MenuItem("Add Point Mass")) {
				Entity point_mass = m_context->create_entity("New Point Mass");
				point_mass.add_component<SpriteRendererComponent>();
				point_mass.add_component<PointMassComponent>();
			}
			if (ImGui::MenuItem("Add Spring")) {
				Entity spring = m_context->create_entity("New Spring");
				spring.add_component<SpringComponent>();
			}
			if (ImGui::MenuItem("Add Cable")) {
				Entity cable = m_context->create_entity("New Cable");
				cable.add_component<CableComponent>();
			}if (ImGui::MenuItem("Add Rod")) {
				Entity rod = m_context->create_entity("New Rod");
				rod.add_component<RodComponent>();
			}
			ImGui::EndPopup();
		}

		ImGui::End();

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

	void SceneHierarchy::draw_entity_node(Entity entity) {
		auto& tag = entity.get_component<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_selection_context == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		static constexpr float button_size = 25;
		bool to_set_context = false;

		ImGui::AlignTextToFramePadding();
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.tag.c_str());
		if (ImGui::IsItemClicked()) {
			to_set_context = true;
			//m_selection_context = entity;
		}
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - button_size);
		if (ImGui::ImageButton((ImTextureID)m_knot_icon->get_rendererID(), ImVec2{ button_size, button_size }, ImVec2{ 0,0 }, ImVec2{ 1,1 }, 0)) {
			if (to_set_context) {
				to_set_context = false;
				
			}
			std::cout << "SET";
		}
		if(to_set_context)
			m_selection_context = entity;
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop");

		

		bool deleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				deleted = true;
			if (ImGui::MenuItem("Duplicate Entity")) {
				if(m_allowed_to_action)
					m_selection_context = m_context->copy_entity(entity);
			}
			ImGui::EndPopup();
		}


		if (opened) {
			if (ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.tag.c_str())) {

				if (ImGui::BeginDragDropSource()) {
					Entity* entity_ = &entity;
					ImGui::SetDragDropPayload("HIERARCHY_ITEM", entity_, sizeof(Entity*) + 8, ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (deleted) {
			m_context->destroy_entity(entity);
			if (m_selection_context == entity)
				m_selection_context = {};
		}
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

		draw_component<TransformComponent>("Transform Component", entity, [](auto& component) {
			float speed = 0.1f;

			//glm::vec2 translation{component.translation.x, component.translation.y};
			draw_vec3_control("Translation", component.translation, speed / 2);
			//component.translation = { translation, 0.0f };


			glm::vec3 rotation = glm::degrees(component.rotation);
			draw_vec3_control("Rotation", rotation, speed / 2);
			component.rotation = glm::radians(rotation);
			draw_vec3_control("Scale", component.scale, speed / 5, 1.0f);
		});

		/*draw_component<CameraComponent>("Camera Component", entity, [](auto& component) {
			auto& camera = component.camera;

			ImGui::Checkbox("Is Primary", &component.active);
			const char* projection_types[2] = { "Perspective", "Orthographic" };
			const char* projection_string = projection_types[(int)camera.get_projection_type()];
			if (ImGui::BeginCombo("Projection Type", projection_string)) {
				for (int i = 0; i < 2; i++) {
					bool is_selected = projection_string == projection_types[i];
					if (ImGui::Selectable(projection_types[i], is_selected)) {
						camera.set_projection_type((SceneCamera::ProjectionType)i);
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.get_projection_type() == SceneCamera::ProjectionType::Perspective) {
				float fov = glm::degrees(camera.get_vertical_fov());
				if (ImGui::DragFloat("Vertical FOV", &fov))
					camera.set_vertical_fov(glm::radians(fov));

				float near_clip = camera.get_perspective_near_clip();
				if (ImGui::DragFloat("Near Clip", &near_clip))
					camera.set_perspective_near_clip(near_clip);

				float far_clip = camera.get_perspective_far_clip();
				if (ImGui::DragFloat("Far Clip", &far_clip))
					camera.set_perspective_far_clip(far_clip);
			}
			else if (camera.get_projection_type() == SceneCamera::ProjectionType::Orthographic) {
				float ortho_size = camera.get_ortho_size();
				if (ImGui::DragFloat("Size", &ortho_size))
					camera.set_ortho_size(ortho_size);

				float near_clip = camera.get_ortho_near_clip();
				if (ImGui::DragFloat("Near Clip", &near_clip))
					camera.set_ortho_near_clip(near_clip);

				float far_clip = camera.get_ortho_far_clip();
				if (ImGui::DragFloat("Far Clip", &far_clip))
					camera.set_ortho_far_clip(far_clip);

				ImGui::Checkbox("Has Fixed Aspect Ratio", &component.fixed_aspect_ratio);
			}
		});*/

		draw_component<SpriteRendererComponent>("Sprite Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			//ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});

		draw_component<CircleRendererComponent>("Circle Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			});

		draw_component<PointMassComponent>("Point Mass Component", entity, [](auto& component) {
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
			draw_vec3_control("Velocity", vel, 0.1/5, 1.0f);
			component.point_mass.set_velocity({ vel.x, vel.y });
			glm::vec3 acc = { component.point_mass.get_acceleration() , 0 };
			draw_vec3_control("Acceleration", acc, 0.1 / 5, 1.0f);
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
					Entity* entity_ = (Entity*)payload->Data;
					//entity_->get_component<TransformComponent>();
					//component.first_object = &entity_->get_component<PointMassComponent>().point_mass;
					component.first_object_id = entity_->get_component<IdComponent>().id;
					//component.first_object = entity_->get_component<PointMassComponent>().point_mass;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					Entity* entity_ = (Entity*)payload->Data;
					//component.second_object = &entity_->get_component<PointMassComponent>().point_mass;
					component.second_object_id = entity_->get_component<IdComponent>().id;
					//component.second_object = entity_->get_component<PointMassComponent>().point_mass;
				}
				ImGui::EndDragDropTarget();
			}
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
					Entity* entity_ = (Entity*)payload->Data;
					component.first_object_id = entity_->get_component<IdComponent>().id;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					Entity* entity_ = (Entity*)payload->Data;
					component.second_object_id = entity_->get_component<IdComponent>().id;
				}
				ImGui::EndDragDropTarget();
			}
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
					Entity* entity_ = (Entity*)payload->Data;
					component.first_object_id = entity_->get_component<IdComponent>().id;
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Text("Attached Object 2");
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag and Drop a Point Mass from the Hierarchy to set this value");
			ImGui::SameLine();
			if (component.second_object_id)
				ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			else
				ImGui::Button("None");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					Entity* entity_ = (Entity*)payload->Data;
					component.second_object_id = entity_->get_component<IdComponent>().id;
				}
				ImGui::EndDragDropTarget();
			}
			});

		draw_component<GravityComponent>("Gravity Component", entity, [](auto& component) {
			ImGui::DragFloat2("Intensity", glm::value_ptr(component.gravity.get_gravity()), 0.0025f);
			});
	}

	void SceneHierarchy::set_selected(Entity entity) {
		m_selection_context = entity;
	}
}