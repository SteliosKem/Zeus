#include "pch.h"
#include "SceneCamera.h"
#include <glm/ext/matrix_clip_space.hpp>

namespace Ivory {
	SceneCamera::SceneCamera() {
		recalculate_projection();
	}

	void SceneCamera::set_orthographic(float size, float near_clip, float far_clip) {
		m_projection_type = ProjectionType::Orthographic;
		m_orthographic_size = size;
		m_orthographic_near = near_clip;
		m_orthographic_far = far_clip;

		recalculate_projection();
	}

	void SceneCamera::set_perspective(float vertical_fov, float near_clip, float far_clip) {
		m_projection_type = ProjectionType::Perspective;
		m_perspective_fov = vertical_fov;
		m_perspective_near = near_clip;
		m_perspective_far = far_clip;
		recalculate_projection();
	}

	void SceneCamera::set_viewport_size(uint32_t width, uint32_t height) {
		m_aspect_ratio = (float)width / height;
		recalculate_projection();
	}

	void SceneCamera::recalculate_projection() {
		if (m_projection_type == ProjectionType::Orthographic) {
			float ortho_left = 0.5f * m_aspect_ratio * -m_orthographic_size;
			float ortho_right = 0.5f * m_aspect_ratio * m_orthographic_size;
			float ortho_bottom = 0.5f * -m_orthographic_size;
			float ortho_top = 0.5f * m_orthographic_size;
			m_projection = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, m_orthographic_near, m_orthographic_far);
		}
		else {
			m_projection = glm::perspective(m_perspective_fov, m_aspect_ratio, m_perspective_near, m_perspective_far);
		}
	}
}