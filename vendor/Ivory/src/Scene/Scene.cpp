#include "pch.h"
#include "Scene.h"
#include "Rendering/Renderer2D.h"
#include "Entity.h"
#include "Components.h"

namespace Ivory {
	Scene::Scene() {
	}

	Entity Scene::create_entity(const std::string& name) {
		return create_entity_with_uuid(Uuid(), name);
	}

	Entity Scene::create_entity_with_uuid(Uuid id, const std::string& name) {
		Entity e{ m_registry.create(), this };
		e.add_component<IdComponent>(id);
		e.add_component<TransformComponent>();
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

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::draw_sprite(transform.get_transform(), sprite, (int)entity);
			if (m_selected && entity == m_selected_entity) {
				tsfm = transform.get_transform();
				quad_transform = &tsfm;
			}
		}

		auto view = m_registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : view) {
			auto& [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
			circ.color = circle.color;
			circ.fade = circle.fade;
			circ.thickness = circle.thickness;
			circ.transform = transform.get_transform();
			circ.entity_id = (int)entity;
			Renderer2D::draw_circle(circ);
			if (m_selected && entity == m_selected_entity)
				circ_select = &circ;
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

	void Scene::on_update_runtime(Timestep dt) {
		m_registry.view<CScriptComponent>().each([=](auto entity, auto& cscript_component) {
			if (!cscript_component.instance) {
				cscript_component.instance = cscript_component.instantiate_script();
				cscript_component.instance->m_entity = Entity{ entity, this };
				cscript_component.instance->on_create();
			}

			cscript_component.instance->on_update(dt);
		});

		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		auto view = m_registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view) {
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.active) {
				main_camera = &camera.camera;
				camera_transform = transform.get_transform();
				break;
			}
		}

		if (main_camera) {
			Renderer2D::begin_scene(main_camera->get_projection(), camera_transform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
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
		}
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

		return new_entity;
	}

	std::shared_ptr<Scene> Scene::copy(const std::shared_ptr<Scene>& scene) {
		std::shared_ptr<Scene> new_scene = std::make_shared<Scene>();

		new_scene->m_vp_height = scene->m_vp_height;
		new_scene->m_vp_width = scene->m_vp_width;

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

		return new_scene;
	}

	void Scene::clear_entities() {
		m_registry.clear();
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
}