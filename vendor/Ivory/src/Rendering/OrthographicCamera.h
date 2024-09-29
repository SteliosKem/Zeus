#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Ivory {
	class OrthographicCamera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		const glm::vec3& get_position() const { return m_position; }
		void set_position(const glm::vec3& pos) { m_position = pos; recalculate_view_matrix(); }

		void set_scale(float scale) { m_scale = scale; recalculate_view_matrix(); }
		float get_scale() const { return m_scale; }

		void set_projection(float left, float right, float bottom, float top);

		float get_rotation() const { return m_zrotation; }
		void set_rotation(float rotation) { m_zrotation = rotation; recalculate_view_matrix(); }

		const glm::mat4& get_projection_matrix() const { return m_projection_matrix; }
		const glm::mat4& get_view_matrix() const { return m_view_matrix; }
		const glm::mat4& get_vp_matrix() const { return m_vp_matrix; }
	private:
		glm::mat4 m_projection_matrix;
		glm::mat4 m_view_matrix;
		glm::mat4 m_vp_matrix;

		glm::vec3 m_position;
		float m_zrotation = 0.0f, m_scale = 1.0f;
		float m_right, m_left, m_top, m_bottom;

		void recalculate_view_matrix();
	};
}