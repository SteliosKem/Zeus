#pragma once
#include "entt.hpp"
#include "Core/Timestep.h"
#include "Rendering/EditorCamera.h"
#include "Core/Uuid.h"
#include "PointMass.h"

namespace Ivory {
	class Entity;

	class Scene {
	public:
		enum SceneType {
			PointMasses,
			Wave
		};
		struct CollisionInfo {
			int32_t frame;
		};

		Scene(SceneType scene_type = PointMasses);
		~Scene() {}

		Entity create_entity(const std::string& name = "", bool no_transform = false);
		Entity create_entity_with_uuid(Uuid id, const std::string& name = "", bool no_transform = false);
		void destroy_entity(Entity entity);
		void on_viewport_resize(uint32_t width, uint32_t height);
		void clear_entities();

		void set_selected_entity(Entity entity);
		void set_time_per_frame(float time) { m_time_per_frame = time; }
		void draw_selection(bool draw, const glm::vec2& point_a = { 0.0f, 0.0f }, const glm::vec2& point_b = {0.0f, 0.0f});
		void remove_selected_entity();

		// Returns true if physics updated this step
		bool on_update_runtime(Timestep dt, EditorCamera& camera, int32_t frame);
		void on_update_editor(Timestep dt, EditorCamera& camera);

		void on_play();

		void pause() { m_paused = true; }
		void unpause() { m_paused = false; }

		Entity get_primary_camera();
		Entity copy_entity(Entity entity);

		Entity get_by_uuid(Uuid& uuid);

		static std::shared_ptr<Scene> copy(const std::shared_ptr<Scene>& scene);
		static std::shared_ptr<Scene> copy(Scene* scene);
		void set_gravity(float g) { m_gravity_force.set_gravity({0.0f, -g}); }
		float get_gravity() { return -m_gravity_force.get_gravity().y; }
		void set_time_factor(float factor) { m_time_factor = factor; }
		float& get_time_factor() { return m_time_factor; }
		const std::vector<CollisionInfo>& get_collisions() { return m_collisions; }
		entt::registry& get_registry() { return m_registry; }
		std::vector<entt::entity>& get_point_mass_entities() { return m_point_mass_entities; }
	private:
		SceneType m_scene_type;

		template<typename T>
		void on_component_add(Entity entity, T& component);

		entt::registry m_registry{};
		
		uint32_t m_vp_width{ 1 }, m_vp_height{ 1 };

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchy;

		bool m_selected = false;
		entt::entity m_selected_entity = {};

		Alchemist::PointMassGravity m_gravity_force;
		float m_time_factor = 1.0f;

		bool m_paused = false;

		// Physics
		Alchemist::ForceRegistry m_force_registry;
		bool on_update_physics(float dt);
		std::vector<entt::entity> m_point_mass_entities;
		float m_time_per_frame = 0.01f;

		int32_t m_current_frame;
		std::vector<CollisionInfo> m_collisions;

		glm::vec2 m_selection[2];
		bool m_draw_selection = false;
	};
}