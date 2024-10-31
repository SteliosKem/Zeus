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
		struct CollisionInfo {
			int32_t frame;
		};

		Scene();
		~Scene() {}

		Entity create_entity(const std::string& name = "");
		Entity create_entity_with_uuid(Uuid id, const std::string& name = "");
		void destroy_entity(Entity entity);
		void on_viewport_resize(uint32_t width, uint32_t height);
		void clear_entities();

		void set_selected_entity(Entity entity);
		void remove_selected_entity();

		void on_update_runtime(Timestep dt, EditorCamera& camera, int32_t frame);
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
	private:
		template<typename T>
		void on_component_add(Entity entity, T& component);

		entt::registry m_registry;
		
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
		void on_update_physics(float dt);
		std::vector<entt::entity> m_point_mass_entities;

		int32_t m_current_frame;
		std::vector<CollisionInfo> m_collisions;
	};
}