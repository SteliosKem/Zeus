#pragma once
#include "Scene/Scene.h"

namespace Ivory {
	class WorldSettings {
	public:
		WorldSettings() {}

		void on_imgui_render();
		void set_scene_ptr(Scene* scene) { m_scene = scene; }

		float& get_gravity() { return m_gravity_intensity; }
		float& get_time_factor() { return m_time_factor; }
	private:
		Scene* m_scene;
		float m_gravity_intensity = 5.0f;
		float m_time_factor = 1.0f;
	};
}