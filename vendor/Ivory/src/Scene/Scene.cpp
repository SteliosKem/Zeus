#include "pch.h"
#include "Scene.h"
#include "Rendering/Renderer2D.h"
#include "Entity.h"
#include "Components.h"

namespace Ivory {
	Scene::Scene(SceneType scene_type) : m_scene_type{scene_type} {
		m_registry = entt::registry();
	}

	Entity Scene::create_entity(const std::string& name, bool no_transform) {
		return create_entity_with_uuid(Uuid(), name, no_transform);
	}

	Entity Scene::create_entity_with_uuid(Uuid id, const std::string& name, bool no_transform) {
		Entity e{ m_registry.create(), this };
		e.add_component<IdComponent>(id);
		if(!no_transform) e.add_component<TransformComponent>();
		e.add_component<TagComponent>(name.empty() ? "New Entity" : name);
		return e;
	}

	void Scene::destroy_entity(Entity entity) {
		m_registry.destroy(entity);
	}

	void Scene::on_update_editor(Timestep dt, EditorCamera& camera) {
		Renderer2D::begin_scene(camera);
		Circle circ{};
		glm::mat4 tsfm;
		Circle* circ_select{ nullptr };
		glm::mat4* quad_transform{ nullptr };

		if (m_draw_selection)
			Renderer2D::draw_selection(m_selection[0], m_selection[1]);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent, PointMassComponent>);
		for (auto entity : group) {
			auto& [transform, sprite, point_mass] = group.get<TransformComponent, SpriteRendererComponent, PointMassComponent>(entity);
			if (point_mass.is_circle) {
				Circle circ{};
				circ.color = sprite.color;
				circ.fade = 0;
				circ.thickness = 1;
				circ.transform = transform.get_transform();
				circ.entity_id = (int)entity;
				Renderer2D::draw_circle(circ);
			}
			else
				Renderer2D::draw_sprite(transform.get_transform(), sprite, (int)entity);
			if (m_selected && entity == m_selected_entity) {
				tsfm = transform.get_transform();
				quad_transform = &tsfm;
			}


			if (point_mass.show_velocity) {
				glm::vec2 vel = point_mass.point_mass.get_velocity();
				//if (glm::length(vel) > 3.0f) {
				vel = glm::normalize(vel);
				vel *= 3;
				//}
				Renderer2D::draw_arrow(transform.translation, transform.translation + glm::vec3{ vel, 0 }, { 0.8, 0.8, 0.8, 1.0f }, (int)entity);
			}

			if (point_mass.show_forces) {
				for (auto& [name, force] : point_mass.impulses) {
					//if (glm::length(vel) > 3.0f) {
					glm::vec2 vec;
					vec = glm::normalize(force);
					vec *= 3;
					//}
					Renderer2D::draw_arrow(transform.translation, transform.translation + glm::vec3{ vec, 0 }, { 0.8, 0.8, 0.8, 1.0f }, (int)entity);
				}
			}
		}

		auto springs = m_registry.view<SpringComponent>();
		for (auto& entity : springs) {
			SpringComponent spring = m_registry.get<SpringComponent>(entity);
			if (spring.first_object_id && spring.second_object_id) {
				TransformComponent t1 = m_registry.get<TransformComponent>(get_by_uuid(spring.first_object_id));
				t1.translation.z = 0;
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(spring.second_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_spring(t1.translation, t2.translation
					, spring.height, spring.revolutions, spring.spring.get_rest_length(), (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		auto anchored_springs = m_registry.view<AnchoredSpringComponent>();
		for (auto& entity : anchored_springs) {
			AnchoredSpringComponent spring = m_registry.get<AnchoredSpringComponent>(entity);
			TransformComponent t1 = m_registry.get<TransformComponent>(entity);
			
			t1.translation.z = 0;
			spring.anchor_object.set_position(t1.translation);
			if (spring.attached_object_id) {
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(spring.attached_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_spring(t1.translation, t2.translation
					, 2.0f, 20, spring.spring.get_rest_length(), (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
			else {
				Renderer2D::draw_spring(t1.translation, { spring.default_object.get_position(), 0 }
					, 2.0f, 20, spring.spring.get_rest_length(), (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		auto cables = m_registry.view<CableComponent>();
		for (auto& entity : cables) {
			CableComponent cable = m_registry.get<CableComponent>(entity);
			if (cable.first_object_id && cable.second_object_id) {
				TransformComponent t1 = m_registry.get<TransformComponent>(get_by_uuid(cable.first_object_id));
				t1.translation.z = 0;
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(cable.second_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_cable(t1.translation, t2.translation, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		auto anchored_cables = m_registry.view<AnchoredCableComponent>();
		for (auto& entity : anchored_cables) {
			TransformComponent t1 = m_registry.get<TransformComponent>(entity);
			t1.translation.z = 0;
			AnchoredCableComponent cable = m_registry.get<AnchoredCableComponent>(entity);
			cable.anchor_object.set_position(t1.translation);

			if (cable.attached_object_id) {
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(cable.attached_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_cable(t1.translation, t2.translation, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
			else {
				Renderer2D::draw_cable(t1.translation, { cable.default_object.get_position(), 0 }, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		auto rods = m_registry.view<RodComponent>();
		for (auto& entity : rods) {
			RodComponent rod = m_registry.get<RodComponent>(entity);
			if (rod.first_object_id && rod.second_object_id) {
				TransformComponent t1 = m_registry.get<TransformComponent>(get_by_uuid(rod.first_object_id));
				t1.translation.z = 0;
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(rod.second_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_cable(t1.translation, t2.translation, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		auto anchored_rods = m_registry.view<AnchoredRodComponent>();
		for (auto& entity : anchored_rods) {
			AnchoredRodComponent rod = m_registry.get<AnchoredRodComponent>(entity);
			TransformComponent t1 = m_registry.get<TransformComponent>(entity);
			t1.translation.z = 0;
			rod.anchor_object.set_position(t1.translation);

			if (rod.attached_object_id) {
				TransformComponent t2 = m_registry.get<TransformComponent>(get_by_uuid(rod.attached_object_id));
				t2.translation.z = 0;
				Renderer2D::draw_cable(t1.translation, t2.translation, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
			else {
				Renderer2D::draw_cable(t1.translation, { rod.default_object.get_position(), 0 }, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
			}
		}

		if(m_selected)
			Renderer2D::draw_overlay(quad_transform, circ_select, (int)m_selected_entity);

		Renderer2D::end_scene();
	}

	void Scene::set_selected_entity(Entity entity) {
		m_selected_entity = entity;
		m_selected = true;
	}

	void Scene::remove_selected_entity() {
		m_selected = false;
	}

	void Scene::draw_selection(bool draw, const glm::vec2& point_a, const glm::vec2& point_b) {
		m_draw_selection = draw;
		m_selection[0] = point_a;
		m_selection[1] = point_b;
	}

	void Scene::on_play() {
		auto springs = m_registry.view<SpringComponent>();
		for (auto entity : springs) {
			auto& comp = springs.get<SpringComponent>(entity);
			if (comp.first_object_id && comp.second_object_id) {
				comp.first_object = &get_by_uuid(comp.first_object_id).get_component<PointMassComponent>().point_mass;
				comp.second_object = &get_by_uuid(comp.second_object_id).get_component<PointMassComponent>().point_mass;
				comp.spring.set_attached_object(comp.first_object);
				m_force_registry.add(comp.second_object, &comp.spring);
			}
		}
		auto anchored_springs = m_registry.view<AnchoredSpringComponent>();
		for (auto entity : anchored_springs) {
			auto& comp = anchored_springs.get<AnchoredSpringComponent>(entity);
			if (comp.attached_object_id) {
				comp.attached_object = &get_by_uuid(comp.attached_object_id).get_component<PointMassComponent>().point_mass;
				comp.spring.set_attached_object(&comp.anchor_object);
				m_force_registry.add(comp.attached_object, &comp.spring);
			}
			else {
				comp.spring.set_attached_object(&comp.anchor_object);
				m_force_registry.add(&comp.default_object, &comp.spring);
			}
		}
		auto cables = m_registry.view<CableComponent>();
		for (auto entity : cables) {
			auto& comp = cables.get<CableComponent>(entity);
			if (comp.first_object_id && comp.second_object_id) {
				comp.first_object = &get_by_uuid(comp.first_object_id).get_component<PointMassComponent>().point_mass;
				comp.second_object = &get_by_uuid(comp.second_object_id).get_component<PointMassComponent>().point_mass;
				comp.cable.first = comp.first_object;
				comp.cable.second = comp.second_object;
				get_by_uuid(comp.first_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(entity).tag] = { 0, 0 };
				get_by_uuid(comp.second_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(entity).tag] = { 0, 0 };
			}
		}
		auto anchored_cables = m_registry.view<AnchoredCableComponent>();
		for (auto entity : anchored_cables) {
			auto& comp = anchored_cables.get<AnchoredCableComponent>(entity);
			if (comp.attached_object_id) {
				comp.attached_object = &get_by_uuid(comp.attached_object_id).get_component<PointMassComponent>().point_mass;
				comp.cable.second = comp.attached_object;
				comp.cable.first = &comp.anchor_object;
			}
			else {
				comp.cable.second = &comp.default_object;
				comp.cable.first = &comp.anchor_object;
			}
		}
		auto rods = m_registry.view<RodComponent>();
		for (auto entity : rods) {
			auto& comp = rods.get<RodComponent>(entity);
			if (comp.first_object_id && comp.second_object_id) {
				comp.first_object = &get_by_uuid(comp.first_object_id).get_component<PointMassComponent>().point_mass;
				comp.second_object = &get_by_uuid(comp.second_object_id).get_component<PointMassComponent>().point_mass;
				comp.rod.first = comp.first_object;
				comp.rod.second = comp.second_object;
				get_by_uuid(comp.first_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(entity).tag] = { 0, 0 };
				get_by_uuid(comp.second_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(entity).tag] = { 0, 0 };
			}
		}
		auto anchored_rods = m_registry.view<AnchoredRodComponent>();
		for (auto entity : anchored_rods) {
			auto& comp = anchored_rods.get<AnchoredRodComponent>(entity);
			if (comp.attached_object_id) {
				comp.attached_object = &get_by_uuid(comp.attached_object_id).get_component<PointMassComponent>().point_mass;
				comp.rod.second = comp.attached_object;
				comp.rod.first = &comp.anchor_object;
			}
			else {
				comp.rod.second = &comp.default_object;
				comp.rod.first = &comp.anchor_object;
			}
		}
		auto point_masses = m_registry.view<PointMassComponent>();
		for (auto entity : point_masses) {
			auto& trans = m_registry.get<TransformComponent>(entity);
			auto& comp = point_masses.get<PointMassComponent>(entity);
			comp.point_mass.set_position(trans.translation);
			if (comp.affected_by_gravity) {
				m_force_registry.add(&comp.point_mass, &m_gravity_force);
			}
			for (auto& force : comp.forces_info) {
				comp.forces.push_back(Alchemist::SimpleForce(force.second.force_vector));
				
			}
			for (auto& force : comp.forces) {
				m_force_registry.add(&comp.point_mass, &force);
			}
			m_point_mass_entities.push_back(entity);
		}
		
	}

	bool Scene::on_update_runtime(Timestep dt, EditorCamera& camera, int32_t frame) {
		m_current_frame = frame;
		bool updated_physics = on_update_physics(m_paused ? 0 : dt * m_time_factor);

		m_registry.view<CScriptComponent>().each([=](auto entity, auto& cscript_component) {
			if (!cscript_component.instance) {
				cscript_component.instance = cscript_component.instantiate_script();
				cscript_component.instance->m_entity = Entity{ entity, this };
				cscript_component.instance->on_create();
			}

			cscript_component.instance->on_update(dt);
		});

		/*Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		auto view = m_registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view) {
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
			if (camera.active) {
				main_camera = &camera.camera;
				camera_transform = transform.get_transform();
				break;
			}
		}*/

		/*if (main_camera) {
			Renderer2D::begin_scene(main_camera->get_projection(), camera_transform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent, PointMassComponent>);
			for (auto entity : group) {
				auto& [transform, sprite, point_mass] = group.get<TransformComponent, SpriteRendererComponent, PointMassComponent>(entity);
				transform.translation = glm::vec3(point_mass.point_mass.get_position(), transform.translation.z);
				Renderer2D::draw_sprite(transform.get_transform(), sprite, (int)entity);
			}

			auto view = m_registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view) {
				auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Circle circ{};
				circ.color = circle.color;
				circ.fade = circle.fade;
				circ.thickness = circle.thickness;
				circ.transform = transform.get_transform();
				circ.entity_id = (int)entity;
				Renderer2D::draw_circle(circ);

			}

			Renderer2D::end_scene();
		}*/
		Renderer2D::begin_scene(camera);

		Circle circ{};
		glm::mat4 tsfm;
		Circle* circ_select{ nullptr };
		glm::mat4* quad_transform{ nullptr };

		if (m_draw_selection)
			Renderer2D::draw_selection(m_selection[0], m_selection[1]);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent, PointMassComponent>);
		for (auto entity : group) {
			auto& [transform, sprite, point_mass] = group.get<TransformComponent, SpriteRendererComponent, PointMassComponent>(entity);
			transform.translation = glm::vec3(point_mass.point_mass.get_position(), transform.translation.z);
			if (point_mass.is_circle) {
				Circle circ{};
				circ.color = sprite.color;
				circ.fade = 0;
				circ.thickness = 1;
				circ.transform = transform.get_transform();
				circ.entity_id = (int)entity;
				Renderer2D::draw_circle(circ);
			}
			else
				Renderer2D::draw_sprite(transform.get_transform(), sprite, (int)entity);
			if (m_selected && entity == m_selected_entity) {
				tsfm = transform.get_transform();
				quad_transform = &tsfm;
			}
			if (point_mass.show_velocity) {
				glm::vec2 vel = point_mass.point_mass.get_velocity();
				//if (glm::length(vel) > 3.0f) {
					vel = glm::normalize(vel);
					vel *= 3;
				//}
				Renderer2D::draw_arrow(transform.translation, transform.translation + glm::vec3{ vel, 0 }, { 0.8, 0.8, 0.8, 1.0f }, (int)entity);
			}
			if (point_mass.show_forces) {
				for (auto& [name, force] : point_mass.impulses) {
					//if (glm::length(vel) > 3.0f) {
					glm::vec2 vec;
					vec = glm::normalize(force);
					vec *= 3;
					//}
					Renderer2D::draw_arrow(transform.translation, transform.translation + glm::vec3{ vec, 0 }, { 0.8, 0.8, 0.8, 1.0f }, (int)entity);
				}
			}
		}

		auto springs = m_registry.view<SpringComponent>();
		for (auto& entity : springs) {
			SpringComponent spring = m_registry.get<SpringComponent>(entity);
			if (spring.first_object_id && spring.second_object_id)
				Renderer2D::draw_spring(glm::vec3(spring.first_object->get_position(), 0.0f), glm::vec3(spring.second_object->get_position(), 0.0f)
				, spring.height, spring.revolutions, spring.spring.get_rest_length(), (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
		}

		auto cables = m_registry.view<CableComponent>();
		for (auto& entity : cables) {
			CableComponent cable = m_registry.get<CableComponent>(entity);
			if (cable.first_object_id && cable.second_object_id)
				Renderer2D::draw_cable({ cable.cable.first->get_position() , 0.0f }, { cable.cable.second->get_position(), 0.0f }, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
		}

		auto rods = m_registry.view<RodComponent>();
		for (auto& entity : rods) {
			RodComponent rod = m_registry.get<RodComponent>(entity);
			if (rod.first_object_id && rod.second_object_id)
				Renderer2D::draw_cable({ rod.rod.first->get_position() , 0.0f }, { rod.rod.second->get_position(), 0.0f }, (m_selected && m_selected_entity == entity) ? glm::vec4(0.8f, 0.2f, 0.1f, 1.0f) : glm::vec4(1.0f), (int)entity);
		}

		if (m_selected)
			Renderer2D::draw_overlay(quad_transform, circ_select, (int)m_selected_entity);

		Renderer2D::end_scene();
		return updated_physics;
	}

	void Scene::on_viewport_resize(uint32_t width, uint32_t height) {
		m_vp_height = height;
		m_vp_width = width;

		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& camera_component = view.get<CameraComponent>(entity);
			if (!camera_component.fixed_aspect_ratio) {
				camera_component.camera.set_viewport_size(width, height);
			}
		}
	}

	Entity Scene::get_primary_camera() {
		auto view = m_registry.view<CameraComponent>();
		for (auto id : view) {
			const auto& camera = view.get<CameraComponent>(id);
			return Entity{ id, this };
			if (camera.active)
				return Entity{ id, this };
		}
		return {};
	}
	template<typename T>
	static void copy_component(entt::registry& source, entt::registry& destination, const std::unordered_map<Uuid, entt::entity>& entity_map) {
		auto view = source.view<T>();
		for (auto e = view.rbegin(); e != view.rend(); e++) {
			
			Uuid uuid = source.get<IdComponent>(*e).id;
			entt::entity destination_entt_id = entity_map.at(uuid);
			auto& component = source.get<T>(*e);
			destination.emplace_or_replace<T>(destination_entt_id, component);
		}
	}

	template<typename T>
	static void copy_component_if_exists(Entity source, Entity destination) {
		if (source.has_component<T>())
			destination.add_or_replace_component<T>(source.get_component<T>());
	}

	Entity Scene::copy_entity(Entity entity) {
		Entity new_entity = create_entity(entity.get_component<TagComponent>().tag + " - Copy");

		copy_component_if_exists<TransformComponent>(entity, new_entity);
		copy_component_if_exists<CameraComponent>(entity, new_entity);
		copy_component_if_exists<SpriteRendererComponent>(entity, new_entity);
		copy_component_if_exists<CircleRendererComponent>(entity, new_entity);
		copy_component_if_exists<CScriptComponent>(entity, new_entity);
		copy_component_if_exists<PointMassComponent>(entity, new_entity);
		copy_component_if_exists<SpringComponent>(entity, new_entity);
		copy_component_if_exists<CableComponent>(entity, new_entity);
		copy_component_if_exists<AnchoredSpringComponent>(entity, new_entity);
		copy_component_if_exists<AnchoredCableComponent>(entity, new_entity);
		copy_component_if_exists<GravityComponent>(entity, new_entity);
		copy_component_if_exists<RodComponent>(entity, new_entity);
		copy_component_if_exists<AnchoredRodComponent>(entity, new_entity);

		return new_entity;
	}

	Entity Scene::get_by_uuid(Uuid& uuid) {
		auto& view = m_registry.view<IdComponent>();
		for (auto& entity : view) {
			auto id = m_registry.get<IdComponent>(entity).id;
			if(id == uuid)
				return Entity{ entity, this };
		}
	}

	std::shared_ptr<Scene> Scene::copy(const std::shared_ptr<Scene>& scene) {
		std::shared_ptr<Scene> new_scene = std::make_shared<Scene>();

		new_scene->m_vp_height = scene->m_vp_height;
		new_scene->m_vp_width = scene->m_vp_width;

		new_scene->m_gravity_force = scene->m_gravity_force;
		new_scene->m_time_factor = scene->m_time_factor;
		new_scene->m_scene_type = scene->m_scene_type;
		new_scene->m_time_per_frame = scene->m_time_per_frame;

		std::unordered_map<Uuid, entt::entity> entity_map;

		auto& source_scene_reg = scene->m_registry;
		auto& dest_scene_reg = new_scene->m_registry;
		auto id_view = source_scene_reg.view<IdComponent>();
		
		for (auto e = id_view.rbegin(); e != id_view.rend(); e++) {
			Uuid uuid = source_scene_reg.get<IdComponent>(*e).id;
			const std::string& name = source_scene_reg.get<TagComponent>(*e).tag;
			Entity entity = new_scene->create_entity_with_uuid(uuid, name);
			entity_map[uuid] = (entt::entity)entity;
		}

		copy_component<TransformComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CameraComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<SpriteRendererComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CircleRendererComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CScriptComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<PointMassComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<SpringComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CableComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<RodComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<GravityComponent>(source_scene_reg, dest_scene_reg, entity_map);

		return new_scene;
	}

	std::shared_ptr<Scene> Scene::copy(Scene* scene) {
		std::shared_ptr<Scene> new_scene = std::make_shared<Scene>();

		new_scene->m_vp_height = scene->m_vp_height;
		new_scene->m_vp_width = scene->m_vp_width;

		new_scene->m_gravity_force = scene->m_gravity_force;
		new_scene->m_time_factor = scene->m_time_factor;

		std::unordered_map<Uuid, entt::entity> entity_map;

		auto& source_scene_reg = scene->m_registry;
		auto& dest_scene_reg = new_scene->m_registry;
		auto id_view = source_scene_reg.view<IdComponent>();

		for (auto e = id_view.rbegin(); e != id_view.rend(); e++) {
			Uuid uuid = source_scene_reg.get<IdComponent>(*e).id;
			const std::string& name = source_scene_reg.get<TagComponent>(*e).tag;
			Entity entity = new_scene->create_entity_with_uuid(uuid, name);
			entity_map[uuid] = (entt::entity)entity;
		}

		copy_component<TransformComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CameraComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<SpriteRendererComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CircleRendererComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CScriptComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<PointMassComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<SpringComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<CableComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<RodComponent>(source_scene_reg, dest_scene_reg, entity_map);
		copy_component<GravityComponent>(source_scene_reg, dest_scene_reg, entity_map);

		return new_scene;
	}

	void Scene::clear_entities() {
		//m_registry.clear();
	}

	bool Scene::on_update_physics(float dt) {
		static float accumulated_time = 0;
		accumulated_time += dt;
		if (accumulated_time > m_time_per_frame) {
			accumulated_time -= m_time_per_frame;
			dt = m_time_per_frame;
			m_force_registry.update_forces(dt);
			auto view = m_registry.view<PointMassComponent>();

			for (auto entity : view) {
				auto& point_mass_component = view.get<PointMassComponent>(entity);
				if (point_mass_component.will_update)
					point_mass_component.point_mass.on_update(dt);
			}
			
			std::unordered_map<entt::entity, entt::entity> collision_history;


			auto cable_view = m_registry.view<CableComponent>();
			for (auto& e : cable_view) {
				CableComponent cable = m_registry.get<CableComponent>(e);
				if (cable.first_object_id && cable.second_object_id) {
					Alchemist::Collision collision;
					collision.restitution = 0.0f;
					collision.first = cable.first_object;
					collision.second = cable.second_object;
					glm::vec2 impulse_vec;
					if (cable.cable.fill_collision(&collision, 1))
						impulse_vec = Alchemist::resolve_collision(cable.first_object, cable.second_object, collision);
					get_by_uuid(cable.first_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(e).tag] = -impulse_vec / dt;
					get_by_uuid(cable.second_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(e).tag] = impulse_vec / dt;
				}
			}
			auto rod_view = m_registry.view<RodComponent>();
			for (auto& e : rod_view) {
				RodComponent rod = m_registry.get<RodComponent>(e);
				if (rod.first_object_id && rod.second_object_id) {
					Alchemist::Collision collision;
					collision.first = rod.first_object;
					collision.second = rod.second_object;
					glm::vec2 impulse_vec;
					if (rod.rod.fill_collision(&collision, 1))
						impulse_vec = Alchemist::resolve_collision(rod.first_object, rod.second_object, collision);
					get_by_uuid(rod.first_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(e).tag] = -impulse_vec / dt;
					get_by_uuid(rod.second_object_id).get_component<PointMassComponent>().impulses[m_registry.get<TagComponent>(e).tag] = impulse_vec / dt;
				}
			}
			for (int i = 0; i < m_point_mass_entities.size() - 1; i++) {
				entt::entity entity = m_point_mass_entities[i];
				view.get<PointMassComponent>(entity).impulses["N"] = { 0, 0 };
			}
			if (m_point_mass_entities.size())
				for (int i = 0; i < m_point_mass_entities.size() - 1; i++) {
					entt::entity entity = m_point_mass_entities[i];
					auto& point_mass1 = view.get<PointMassComponent>(entity);
					
					if (point_mass1.ignore_collisions)
						continue;
					for (int j = i + 1; j < m_point_mass_entities.size(); j++) {
						entt::entity entity2 = m_point_mass_entities[j];
						auto& point_mass2 = view.get<PointMassComponent>(entity2);
						if (point_mass2.ignore_collisions)
							continue;
						if (point_mass1.point_mass.is_static() && point_mass2.point_mass.is_static())
							continue;
						const auto& transform1 = m_registry.get<TransformComponent>(entity);
						const auto& transform2 = m_registry.get<TransformComponent>(entity2);
						Alchemist::Collision collision;
						if (point_mass1.is_circle && point_mass2.is_circle)
							collision = Alchemist::check_circle_collision_depth(point_mass1.point_mass, point_mass2.point_mass);
						else if (!point_mass1.is_circle && !point_mass2.is_circle)
							collision = Alchemist::check_sat_collision(transform1.get_transformed_points(), transform2.get_transformed_points());
						else {
							if (point_mass1.is_circle)
								collision = Alchemist::check_sat_circle_collision(point_mass1.point_mass.get_position(), 0.5f, transform2.get_transformed_points());
							else
								collision = Alchemist::check_sat_circle_collision(point_mass2.point_mass.get_position(), 0.5f, transform1.get_transformed_points());
						}


						float restitution = fminf(point_mass1.point_mass.get_restitution(), point_mass2.point_mass.get_restitution());
						collision.restitution = restitution;

						glm::vec2 impulse_vec;

						if (collision.depth != 0) {
							impulse_vec = Alchemist::resolve_collision_with_friction(&point_mass1.point_mass, &point_mass2.point_mass, collision);
							m_registry.get<PointMassComponent>(entity).impulses["N"] -= impulse_vec / dt;
							m_registry.get<PointMassComponent>(entity2).impulses["N"] += impulse_vec / dt;
						}
					}
				}


			
			return true;
		}
		return false;
	}

	template<typename T>
	void Scene::on_component_add(Entity entity, T& component) {
		static_assert(false);
	}

	template<>
	void Scene::on_component_add<TransformComponent>(Entity entity, TransformComponent& component) {}

	template<>
	void Scene::on_component_add<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {}

	template<>
	void Scene::on_component_add<TagComponent>(Entity entity, TagComponent& component) {}

	template<>
	void Scene::on_component_add<CScriptComponent>(Entity entity, CScriptComponent& component) {}

	template<>
	void Scene::on_component_add<IdComponent>(Entity entity, IdComponent& component) {}

	template<>
	void Scene::on_component_add<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {}

	template<>
	void Scene::on_component_add<CameraComponent>(Entity entity, CameraComponent& component) {
		component.camera.set_viewport_size(m_vp_width, m_vp_height);
	}

	template<>
	void Scene::on_component_add<PointMassComponent>(Entity entity, PointMassComponent& component) {}

	template<>
	void Scene::on_component_add<SpringComponent>(Entity entity, SpringComponent& component) {}

	template<>
	void Scene::on_component_add<AnchoredSpringComponent>(Entity entity, AnchoredSpringComponent& component) {}

	template<>
	void Scene::on_component_add<CableComponent>(Entity entity, CableComponent& component) {}

	template<>
	void Scene::on_component_add<AnchoredCableComponent>(Entity entity, AnchoredCableComponent& component) {}

	template<>
	void Scene::on_component_add<RodComponent>(Entity entity, RodComponent& component) {}

	template<>
	void Scene::on_component_add<AnchoredRodComponent>(Entity entity, AnchoredRodComponent& component) {}

	template<>
	void Scene::on_component_add<GravityComponent>(Entity entity, GravityComponent& component) {}
}