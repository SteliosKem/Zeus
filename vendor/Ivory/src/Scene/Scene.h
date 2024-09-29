#pragma once
#include "entt.hpp"
#include "Core/Timestep.h"
#include "Rendering/EditorCamera.h"
#include "Core/Uuid.h"

namespace Ivory {
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene() {}

		Entity create_entity(const std::string& name = "");
		Entity create_entity_with_uuid(Uuid id, const std::string& name = "");
		void destroy_entity(Entity entity);
		void on_viewport_resize(uint32_t width, uint32_t height);
		void clear_entities();

		void set_selected_entity(Entity entity);
		void remove_selected_entity();

		void on_update_runtime(Timestep dt);
		void on_update_editor(Timestep dt, EditorCamera& camera);

		Entity get_primary_camera();
		Entity copy_entity(Entity entity);

		static std::shared_ptr<Scene> copy(const std::shared_ptr<Scene>& scene);
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
	};
}