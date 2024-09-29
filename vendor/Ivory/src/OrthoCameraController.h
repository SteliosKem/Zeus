#pragma once
#include "Rendering/OrthographicCamera.h"
#include "Core/Timestep.h"

#include "Events/WindowEvent.h"
#include "Events/MouseEvent.h"

namespace Ivory {
	class OrthographicCameraController {
	public:
		OrthographicCameraController(float aspect_ratio);

		void on_update(Timestep dt);
		void on_event(Event& e);

		void resize_bounds(float width, float height);

		OrthographicCamera& get_camera() { return m_camera; }
		const OrthographicCamera& get_camera() const { return m_camera; }

		float set_zoom_leve(float level) { m_zoom_level = level; calculate_view(); }
		void pass_events(bool pass) { m_pass_events = pass; }
	private:
		bool on_mouse_scroll(MouseScrollEvent& e);
		bool on_window_resize(WindowResizeEvent& e);

		void calculate_view();

		float m_aspect_ratio;
		float m_zoom_level = 1.0f;
		bool m_pass_events = true;
		OrthographicCamera m_camera;
		
		glm::vec2 m_last_mouse_pos{ 1.0f };

		glm::vec3 m_camera_pos{ 0.0f };
	};
}