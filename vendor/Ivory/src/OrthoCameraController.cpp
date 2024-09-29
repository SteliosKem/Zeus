#include "pch.h"
#include "OrthoCameraController.h"
#include "Core/Input.h"

#include "imgui.h"

namespace Ivory {
	OrthographicCameraController::OrthographicCameraController(float aspect_ratio) : m_aspect_ratio(aspect_ratio), 
		m_camera(-aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level){

	}

	void OrthographicCameraController::on_update(Timestep dt) {
		glm::vec2 mouse_pos = Input::mouse_pos();
		if (Input::is_mouse_button_pressed(2) && m_pass_events) {
			//IV_INFO(m_mouse_pos.x - m_last_mouse_pos.x);
			//IV_INFO(m_mouse_pos.y - m_last_mouse_pos.y);
			m_camera_pos.x += -(mouse_pos.x - m_last_mouse_pos.x) * 0.0032f * m_zoom_level;
			m_camera_pos.y += (mouse_pos.y - m_last_mouse_pos.y) * 0.0032f * m_zoom_level;
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		m_last_mouse_pos = mouse_pos;
		m_camera.set_position(m_camera_pos);
	}
	void OrthographicCameraController::on_event(Event& e) {
		Ivory::EventDispatcher dispatcher(e);

		dispatcher.dispatch<Ivory::MouseScrollEvent>(std::bind(&OrthographicCameraController::on_mouse_scroll, this, std::placeholders::_1));
		dispatcher.dispatch<Ivory::WindowResizeEvent>(std::bind(&OrthographicCameraController::on_window_resize, this, std::placeholders::_1));
	}

	bool OrthographicCameraController::on_mouse_scroll(MouseScrollEvent& e) {
		if(m_pass_events)
			e.get_offset_y() > 0 ? m_zoom_level /= 1.25 : m_zoom_level *= 1.25;
		calculate_view();
		return false;
	}
	bool OrthographicCameraController::on_window_resize(WindowResizeEvent& e) {
		m_aspect_ratio = (float)e.get_width() / (float)e.get_height();
		calculate_view();

		return false;
	}

	void OrthographicCameraController::calculate_view() {
		m_camera.set_projection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level);
	}

	void OrthographicCameraController::resize_bounds(float width, float height) {
		m_aspect_ratio = width / height;
		calculate_view();
	}
}