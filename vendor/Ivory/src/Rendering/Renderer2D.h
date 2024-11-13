#pragma once
#include "OrthographicCamera.h"
#include "Texture.h"
#include "SubTexture.h"
#include "Camera.h"
#include "EditorCamera.h"
#include "Scene/Components.h"


namespace Ivory {
	struct TextureInfo {
		float tiling_factor = 1.0f;
	};

	struct Quad {
		glm::vec3 position{0.0f, 0.0f, 0.0f};
		glm::vec2 size{ 1.0f, 1.0f };
		float rotation{ 0.0f };
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<Texture2D> texture{ nullptr };
		std::shared_ptr<SubTexture2D> sub_texture{ nullptr };
		TextureInfo texture_info{};
		glm::mat4 transform{1.0f};
		int entity_id = -1;
	};

	struct Circle {
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::mat4 transform{ 1.0f };
		float thickness = 1.0f;
		float fade = 0.0f;
		int entity_id = -1;
	};

	class Renderer2D {
	public:
		static void init();
		static void shutdown();

		static void begin_scene(const OrthographicCamera& camera);
		static void begin_scene(const EditorCamera& camera);
		static void begin_scene(const Camera& camera, const glm::mat4& transform);
		static void end_scene();

		static void flush();

		static void draw_quad(const Quad& quad);
		static void draw_circle(const Circle& circle);
		static void draw_sprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id);
		static void draw_line(const glm::vec3& start_pos, const glm::vec3& end_pos, const glm::vec4& color, int entity_id);
		static void draw_line_rectangle(const glm::mat4& transform, const glm::vec4& color, int entity_id);
		static void draw_line_rectangle(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, int entity_id);
		static void draw_spring(const glm::vec3& pos_a, const glm::vec3& pos_b, float height, int revolutions, float rest_length, const glm::vec4& color, int entity_id);
		static void draw_cable(const glm::vec3& pos_a, const glm::vec3& pos_b, const glm::vec4& color, int entity_id);

		static void draw_overlay(glm::mat4* transform, Circle* circle, int entity_id);
		//static void apply_vignette(uint32_t id, float strength);

		static void set_line_width(float width);
		static float get_line_width();

		struct Statistics {
			uint32_t draw_calls = 0;
			uint32_t quad_count = 0;

			uint32_t get_vertex_count() { return quad_count * 4; }
			uint32_t get_index_count() { return quad_count * 6; }
		};
		static void reset_stats();
		static const Statistics& get_stats();
	private:
		static void draw_overlay();
		
		static void new_batch();
		static void start_batch();
	};
}