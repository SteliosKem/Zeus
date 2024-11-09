#pragma once
#include <memory>
#include "Logging/Log.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include <Core/IvoryEngine.h>

namespace Ivory {
	class SceneHierarchy {
	public:
		SceneHierarchy() = default;
		SceneHierarchy(const std::shared_ptr<Scene>& scene);
		void set_context(const std::shared_ptr<Scene>& scene);
		void set_allow_action_ptr(bool allow) { m_allowed_to_action = allow; }
		void empty_selection() { m_selection_context = {}; }
		void start_up() { m_knot_icon = Texture2D::create("Assets/EditorIcons/knot.png"); }

		void on_imgui_render(bool is_playing);
		Entity get_selected() const { return m_selection_context; }
		void set_selected(Entity entity);
	private:
		void draw_entity_node(Entity entity);
		void draw_components(Entity entity);
		bool m_is_playing;

		bool m_allowed_to_action;

		std::shared_ptr<Texture2D> m_knot_icon;
		std::shared_ptr<Scene> m_context;
		Entity m_selection_context;
	};
}