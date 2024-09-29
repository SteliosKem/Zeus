#include "pch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Ivory {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_projection_matrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_position(0, 0, 0), m_view_matrix(1.0f) {
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_vp_matrix = m_projection_matrix * m_view_matrix;
	}

	void OrthographicCamera::recalculate_view_matrix() {
		glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(m_scale)) * glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_zrotation), glm::vec3(0.0f, 0.0f, 1.0f));
		m_view_matrix = glm::inverse(transform);
		m_vp_matrix = m_projection_matrix * m_view_matrix;
	}

	void OrthographicCamera::set_projection(float left, float right, float bottom, float top) {
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_projection_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_vp_matrix = m_projection_matrix * m_view_matrix;
	}
}