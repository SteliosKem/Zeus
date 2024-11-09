#include "pch.h"
#include "EditorCamera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "Core/Input.h"
#include "Core/MouseButtonCodes.h"
#include "Core/KeyCodes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Ivory {
	EditorCamera::EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip)
		: m_FOV(fov), m_aspect_ratio(aspect_ratio), m_near_clip(near_clip), m_far_clip(far_clip), Camera(glm::perspective(glm::radians(fov), aspect_ratio, near_clip, far_clip)) {
		update_view();
	}

	void EditorCamera::update_projection() {
		m_aspect_ratio = m_viewport_width / m_viewport_height;
		m_projection = glm::perspective(glm::radians(m_FOV), m_aspect_ratio, m_near_clip, m_far_clip);
	}

	void EditorCamera::update_view() {
		m_position = calculate_position();

		glm::quat orientation = get_orientation();
		m_view_matrix = glm::translate(glm::mat4{ 1.0f }, m_position) * glm::toMat4(orientation);
		m_view_matrix = glm::inverse(m_view_matrix);
	}

	glm::vec2 EditorCamera::pan_speed() const {
		glm::vec2 vector;
		float x = std::min(m_viewport_width / 1000.0f, 2.4f);
		vector.x = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_viewport_height / 1000.0f, 2.4f);
		vector.y = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return vector;
	}

	float EditorCamera::rotation_speed() const { return 0.8f; }
	float EditorCamera::zoom_speed() const {
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}

	void EditorCamera::on_update(Timestep dt) {
		const glm::vec2& mouse{ Input::mouse_pos()};
		glm::vec2 delta = (mouse - m_initial_mouse_pos) * 0.003f;
		m_initial_mouse_pos = mouse;

		if (Input::is_mouse_button_pressed(IV_MOUSE_BUTTON_3)) {
			mouse_pan(delta);
			update_view();
		}
	}

	void EditorCamera::on_event(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(IV_BIND_EVENT_FN(EditorCamera::on_mouse_scroll));
	}

	bool EditorCamera::on_mouse_scroll(MouseScrollEvent& e) {
		if (m_allow_scroll) {
			float delta = e.get_offset_y() * 0.1f;
			mouse_zoom(delta);
			update_view();
		}
		return false;
	}

	void EditorCamera::mouse_pan(const glm::vec2& delta) {
		m_focal_point += -get_right_direction() * delta.x * pan_speed().x * m_distance;
		m_focal_point += get_up_direction() * delta.y * pan_speed().y * m_distance;
	}

	void EditorCamera::mouse_rotate(const glm::vec2& delta) {
		
	}

	void EditorCamera::mouse_zoom(float delta) {
		m_distance -= delta * zoom_speed();
		if (m_distance < 1.0f) {
			m_focal_point += get_forward_direction();
			m_distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::get_up_direction() const { return glm::rotate(get_orientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::vec3 EditorCamera::get_right_direction() const { return glm::rotate(get_orientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 EditorCamera::get_forward_direction() const { return glm::rotate(get_orientation(), glm::vec3(0.0f, 0.0f, -1.0f)); }

	glm::vec3 EditorCamera::calculate_position() const { return m_focal_point - get_forward_direction() * m_distance; }
	glm::quat EditorCamera::get_orientation() const { return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f)); }
}