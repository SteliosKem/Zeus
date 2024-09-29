#pragma once
#include "Rendering/Camera.h"

namespace Ivory {
	class SceneCamera : public Camera {
	public:
		SceneCamera();
		~SceneCamera() = default;

		void set_viewport_size(uint32_t width, uint32_t height);

		enum class ProjectionType {
			Perspective = 0,
			Orthographic = 1
		};

		ProjectionType get_projection_type() const { return m_projection_type; }
		void set_projection_type(ProjectionType type) { m_projection_type = type; recalculate_projection(); }

		// ORTHOGRAPHIC

		void set_orthographic(float size, float near_clip, float far_clip);

		float get_ortho_size() const { return m_orthographic_size; }
		void set_ortho_size(float size) { m_orthographic_size = size; recalculate_projection(); }

		float get_ortho_near_clip() const { return m_orthographic_near; }
		float get_ortho_far_clip() const { return m_orthographic_far; }

		void set_ortho_near_clip(float clip) { m_orthographic_near = clip; recalculate_projection(); }
		void set_ortho_far_clip(float clip) { m_orthographic_far = clip; recalculate_projection(); }

		// PERSPECTIVE

		void set_perspective(float vertical_fov, float near_clip, float far_clip);

		float get_vertical_fov() const { return m_perspective_fov; }
		void set_vertical_fov(float vertical_fov) { m_perspective_fov = vertical_fov; recalculate_projection(); }

		float get_perspective_near_clip() const { return m_perspective_near; }
		float get_perspective_far_clip() const { return m_perspective_far; }

		void set_perspective_near_clip(float clip) { m_perspective_near = clip; recalculate_projection(); }
		void set_perspective_far_clip(float clip) { m_perspective_far = clip; recalculate_projection(); }


	private:
		void recalculate_projection();

		ProjectionType m_projection_type = ProjectionType::Perspective;

		float m_orthographic_size = 10.0f;
		float m_orthographic_near = -1.0f;
		float m_orthographic_far = 1.0f;

		float m_perspective_fov = glm::radians(45.0f);
		float m_perspective_near = 0.01f;
		float m_perspective_far = 10000.0f;

		float m_aspect_ratio{ 0.0f };
	};
}