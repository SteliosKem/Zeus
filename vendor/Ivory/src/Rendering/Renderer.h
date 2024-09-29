#pragma once
#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Ivory {
	class Renderer {
	public:
		static void init();
		static void on_window_resize(uint32_t width, uint32_t height);

		static void begin_scene(OrthographicCamera& camera);
		static void end_scene();

		static void submit(const std::shared_ptr<VertexArray>& vertex_array, const std::shared_ptr<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }
		//inline static void set_api(RendererAPI::API api) { s_renderer_api = api; }
	private:
		struct SceneData {
			glm::mat4 vp_matrix;
		};

		static std::shared_ptr<SceneData> s_scene_data;
	};
}