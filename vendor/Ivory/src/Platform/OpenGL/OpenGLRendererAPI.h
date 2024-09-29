#pragma once

#include "Rendering/RendererAPI.h"

namespace Ivory {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		void init() override;
		void set_clear_color(const glm::vec4& color) override;
		void clear() override;
		void disable_depth() override;
		void enable_depth() override;

		void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array, uint32_t index_count = 0) override;
		void draw_lines(const std::shared_ptr<VertexArray>& vertex_array, uint32_t vertex_count) override;
		void set_line_width(float width) override;
	};
}