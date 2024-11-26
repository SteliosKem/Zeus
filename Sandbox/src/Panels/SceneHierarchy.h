#pragma once
#include <memory>
#include "Logging/Log.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include <Core/IvoryEngine.h>
#include "Grapher.h"
#include <variant>

namespace Ivory {
	class SceneHierarchy {
	public:
		SceneHierarchy() = default;
		SceneHierarchy(const std::shared_ptr<Scene>& scene);
		void set_context(const std::shared_ptr<Scene>& scene);
		void set_allow_action_ptr(bool allow) { m_allowed_to_action = allow; }
		void empty_selection() { m_selection_context = {}; }
		void start_up() { m_knot_icon = Texture2D::create("Assets/EditorIcons/knot.png"); }

		void on_imgui_render(bool is_playing, bool show_inspector, bool show_hierarchy);
		Entity get_selected() const { return m_selection_context; }
		void set_selected(Entity entity);
		void set_grapher(Grapher* grapher) { m_grapher = grapher; }
	private:
		void draw_entity_node(Entity entity, bool even = false);
		void draw_components(Entity entity);
		void add_to_graph(Entity entity, const std::string& quantity, Quantity quantity_type);
		void draw_vec3_internal(Entity entity, const std::string& label, glm::vec3& values, float speed, float reset_value = 0.0f, float column_width = 120.0f);
		void draw_create_menu();
		void attach_drag_drop(SpringComponent& component, bool first_attachment);
		void attach_drag_drop(CableComponent& component, bool first_attachment);
		void attach_drag_drop(RodComponent& component, bool first_attachment);

		bool m_is_playing;

		bool m_allowed_to_action;

		std::shared_ptr<Texture2D> m_knot_icon;
		std::shared_ptr<Scene> m_context;
		Grapher* m_grapher;
		Entity m_selection_context;
	};
}