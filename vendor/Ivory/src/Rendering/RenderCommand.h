#pragma once
#include "RendererAPI.h"

namespace Ivory {
	class RenderCommand {
	public:
		inline static void init() { s_rendererAPI->init(); }
		inline static void set_clear_color(const glm::vec4& color) { s_rendererAPI->set_clear_color(color); }
		inline static void clear() { s_rendererAPI->clear(); }
		inline static void disable_depth() { s_rendererAPI->disable_depth(); }
		inline static void enable_depth() { s_rendererAPI->enable_depth(); }

		inline static void set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_rendererAPI->set_viewport(x, y, width, height); }

		inline static void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array, uint32_t count = 0) {
			s_rendererAPI->draw_indexed(vertex_array, count);
		}
		inline static void draw_lines(const std::shared_ptr<VertexArray>& vertex_array, uint32_t vertex_count) {
			s_rendererAPI->draw_lines(vertex_array, vertex_count);
		}
		inline static void set_line_width(float width) {
			s_rendererAPI->set_line_width(width);
		}
	private:
		static std::shared_ptr<RendererAPI> s_rendererAPI;
	};
}