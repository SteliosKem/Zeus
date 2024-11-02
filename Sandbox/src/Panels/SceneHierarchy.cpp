#include "SceneHierarchy.h"
#include "imgui.h"
#include "Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui_internal.h"
#include <filesystem>
#include "Project/Project.h"
#include "ImGui/ImGuiNotify.h"

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
		if (entity.has_component<T>()) {
			if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth, name.c_str())) {
				bool deleted = false;
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Delete Component"))
						deleted = true;
					ImGui::EndPopup();
				}
				auto& component = entity.get_component<T>();
				ui_func(component);

				ImGui::TreePop();
				if (deleted)
					entity.remove_component<T>();
			}
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
			if (ImGui::MenuItem("Create Entity"))
				m_context->create_entity();
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
				rod.add_component<CableComponent>();
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
		else
			ImGui::Text("Select a scene entity to inspect its elements");
		ImGui::End();
	}

	void SceneHierarchy::draw_entity_node(Entity entity) {
		auto& tag = entity.get_component<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_selection_context == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.tag.c_str());

		if (ImGui::IsItemClicked()) {
			m_selection_context = entity;
		}

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

	static void draw_vec3_control(const std::string& label, glm::vec3& values, float speed,  float reset_value = 0.0f, float column_width = 100.0f) {
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
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 37/256.0f, 170/256.0f, 37/256.0f, 1.0f });
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

	void SceneHierarchy::draw_components(Entity entity) {
		const ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen;

		if (entity.has_component<TagComponent>()) {
			auto& tag = entity.get_component<TagComponent>();
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.tag.c_str());
			if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
				tag.tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				m_selection_context.add_component<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
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
			if (ImGui::MenuItem("Gravity")) {
				m_selection_context.add_component<GravityComponent>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		draw_component<TransformComponent>("Transform Component", entity, [](auto& component) {
			float speed = 0.1f;

			draw_vec3_control("Translation", component.translation, speed / 2);
			glm::vec3 rotation = glm::degrees(component.rotation);
			draw_vec3_control("Rotation", rotation, speed * 5);
			component.rotation = glm::radians(rotation);
			draw_vec3_control("Scale", component.scale, speed / 5, 1.0f);
		});

		draw_component<CameraComponent>("Camera Component", entity, [](auto& component) {
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
		});

		draw_component<SpriteRendererComponent>("Sprite Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			if (component.texture)
				ImGui::ImageButton((ImTextureID)component.texture->get_rendererID(), ImVec2{ 100.0f, 100.0f }, {0,1}, {1,0});
			else
				ImGui::Button("Texture", ImVec2{ 100.0f, 100.0f });
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					component.texture = Texture2D::create((std::filesystem::path("Assets") / path).string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::DragFloat("Tiling Factor", &component.tiling_factor, 0.1f, 0.0f);
		});

		draw_component<CircleRendererComponent>("Circle Renderer Component", entity, [](auto& component) {
			ImGui::ColorPicker4("Color", glm::value_ptr(component.color));
			ImGui::DragFloat("Thickness", &component.thickness, 0.0025f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.fade, 0.0025f, 0.0f, 1.0f);
			});

		draw_component<PointMassComponent>("Point Mass Component", entity, [](auto& component) {
			float mass = component.point_mass.get_mass();
			ImGui::DragFloat("Mass", &mass, 0.05f, 0.0f);
			component.point_mass.set_mass(mass);
			ImGui::DragFloat("Restitution", &component.point_mass.get_restitution(), 0.05f, 0.0f);

			bool is_static = component.point_mass.is_static();
			ImGui::Checkbox("Is Static", &is_static);
			component.point_mass.set_static(is_static);

			bool only_plain_collision = component.point_mass.does_only_collide_plainly();
			ImGui::Checkbox("Does only collide plainly", &only_plain_collision);
			component.point_mass.set_only_collide_plainly(only_plain_collision);
			ImGui::Checkbox("Affected by Gravity", &component.affected_by_gravity);

			ImGui::DragFloat2("Position", glm::value_ptr(component.point_mass.get_position()), 0.1f);
			ImGui::DragFloat2("Velocity", glm::value_ptr(component.point_mass.get_velocity()), 0.1f);
			ImGui::DragFloat2("Acceleration", glm::value_ptr(component.point_mass.get_acceleration()), 0.1f);

			ImGui::DragFloat("Damping", &component.point_mass.get_damping(), 0.0025f);

			ImGui::Checkbox("Ignore Collisions", &component.ignore_collisions);
			ImGui::Checkbox("Is Circle", &component.is_circle);

			if (ImGui::Button("Add Force")) {
				component.forces_info["Force " + std::to_string(component.force_counter++)];
			}

			for (auto& [name, force] : component.forces_info) {
				ImGui::Text(name.c_str());
				ImGui::SameLine();
				ImGui::DragFloat2(("##" + name).c_str(), glm::value_ptr(component.forces_info[name].force_vector), 0.1f);
			}
		});

		draw_component<SpringComponent>("Spring Component", entity, [](auto& component) {
			ImGui::DragFloat("Rest Length", &component.spring.get_rest_length(), 0.1f, 0.0f);
			ImGui::DragFloat("Spring Constant", &component.spring.get_constant(), 0.1f, 0.0f);

			ImGui::Checkbox("Force Acting on Both Objects", &component.spring.get_first_object_affected());
			//if(component.first_object_id)
			//	ImGui::Button(m_context->get_by_uuid(component.first_object_id).get_component<TagComponent>().tag.c_str());
			//else
				ImGui::Button("Attached Object 1");
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
			
			//if (component.second_object_id)
			//	ImGui::Button(m_context->get_by_uuid(component.second_object_id).get_component<TagComponent>().tag.c_str());
			//else
				ImGui::Button("Attached Object 2");
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

		draw_component<CableComponent>("Cable Component", entity, [](auto& component) {
			ImGui::DragFloat("Max Length", &component.cable.get_max_length(), 0.1f, 0.0f);

			ImGui::Button("Attached Object 1");
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM")) {
					Entity* entity_ = (Entity*)payload->Data;
					component.first_object_id = entity_->get_component<IdComponent>().id;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Button("Attached Object 2");
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