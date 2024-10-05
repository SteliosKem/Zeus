#include "pch.h"
#include "Renderer2D.h"
#include "Buffer.h"
#include "Shader.h"
#include "Scene/Components.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "RenderCommand.h"

namespace Ivory {
	struct QuadVertex {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texture_coord;
		float texture_index;
		float tiling_factor;

		//Editor only
		int entity_id = 0;
	};

	struct CircleVertex {
		glm::vec3 world_position;
		glm::vec3 local_position;
		glm::vec4 color;
		float thickness;
		float fade;

		//Editor only
		int entity_id = 0;
	};

	struct LineVertex {
		glm::vec3 position;
		glm::vec4 color;

		//Editor only
		int entity_id = 0;
	};

	struct Renderer2DStorage {
		const uint32_t max_quads = 10000;
		const uint32_t max_vertices = 10000 * 4;
		static const uint32_t max_indices = 10000 * 6;
		static const uint32_t max_texture_slots = 32;

		std::shared_ptr<VertexArray> quad_vertex_array;
		std::shared_ptr<VertexBuffer> quad_vertex_buffer;
		std::shared_ptr<Shader> texture_shader;
		std::shared_ptr<Texture2D> white_texture;

		std::shared_ptr<VertexArray> circle_vertex_array;
		std::shared_ptr<VertexBuffer> circle_vertex_buffer;
		std::shared_ptr<Shader> circle_shader;

		std::shared_ptr<VertexArray> line_vertex_array;
		std::shared_ptr<VertexBuffer> line_vertex_buffer;
		std::shared_ptr<Shader> line_shader;

		float line_width = 2.0f;

		uint32_t quad_index_count = 0;
		QuadVertex* quad_vertex_buffer_base = nullptr;
		QuadVertex* quad_vertex_buffer_ptr = nullptr;

		uint32_t circle_index_count = 0;
		CircleVertex* circle_vertex_buffer_base = nullptr;
		CircleVertex* circle_vertex_buffer_ptr = nullptr;

		uint32_t line_vertex_count = 0;
		LineVertex* line_vertex_buffer_base = nullptr;
		LineVertex* line_vertex_buffer_ptr = nullptr;

		std::array<std::shared_ptr<Texture2D>, max_texture_slots> texture_slots;
		uint32_t texture_slot_index = 1;

		glm::vec4 quad_vertex_positions[4];

		Renderer2D::Statistics statistics;

		// Overlay
		glm::mat4* overlay_transform;
		Circle* overlay_circle;
		int overlay_entity_id;
	};

	static Renderer2DStorage s_data;

	void Renderer2D::draw_overlay(glm::mat4* transform, Circle* circle, int entity_id) {
		s_data.overlay_transform = transform;
		s_data.overlay_circle = circle;
		s_data.overlay_entity_id = entity_id;
	}

	void Renderer2D::init() {
		s_data.quad_vertex_array = VertexArray::create_array();

		s_data.quad_vertex_buffer = VertexBuffer::create_buffer(s_data.max_vertices * sizeof(QuadVertex));

		BufferLayout solid_layout = {
			{ShaderDataType::Vector3, "a_position"},
			{ShaderDataType::Vector4, "a_color"},
			{ShaderDataType::Vector2, "a_texture_coord"},
			{ShaderDataType::Float, "a_texture_index"},
			{ShaderDataType::Float, "a_tiling_factor"},
			{ShaderDataType::Int, "a_entity_id"},
		};
		s_data.quad_vertex_buffer->set_layout(solid_layout);
		s_data.quad_vertex_array->add_vertex_buffer(s_data.quad_vertex_buffer);

		s_data.quad_vertex_buffer_base = new QuadVertex[s_data.max_vertices];

		uint32_t* quad_indices = new uint32_t[s_data.max_indices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.max_indices; i += 6) {
			quad_indices[i] = offset;
			quad_indices[i + 1] = offset + 1;
			quad_indices[i + 2] = offset + 2;
			quad_indices[i + 3] = offset + 2;
			quad_indices[i + 4] = offset + 3;
			quad_indices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quad_IB(IndexBuffer::create_buffer(quad_indices, s_data.max_indices));
		s_data.quad_vertex_array->set_index_buffer(quad_IB);
		delete[] quad_indices;

		s_data.circle_vertex_array = VertexArray::create_array();
		s_data.circle_vertex_buffer = VertexBuffer::create_buffer(s_data.max_vertices * sizeof(CircleVertex));

		BufferLayout circle_layout = {
			{ShaderDataType::Vector3, "a_world_position"},
			{ShaderDataType::Vector3, "a_local_position"},
			{ShaderDataType::Vector4, "a_color"},
			{ShaderDataType::Float, "a_thickness"},
			{ShaderDataType::Float, "a_fade"},
			{ShaderDataType::Int, "a_entity_id"},
		};
		s_data.circle_vertex_buffer->set_layout(circle_layout);
		s_data.circle_vertex_array->add_vertex_buffer(s_data.circle_vertex_buffer);
		s_data.circle_vertex_array->set_index_buffer(quad_IB);
		s_data.circle_vertex_buffer_base = new CircleVertex[s_data.max_vertices];

		s_data.line_vertex_array = VertexArray::create_array();
		s_data.line_vertex_buffer = VertexBuffer::create_buffer(s_data.max_vertices * sizeof(LineVertex));

		BufferLayout line_layout = {
			{ShaderDataType::Vector3, "a_position"},
			{ShaderDataType::Vector4, "a_color"},
			{ShaderDataType::Int, "a_entity_id"},
		};
		s_data.line_vertex_buffer->set_layout(line_layout);
		s_data.line_vertex_array->add_vertex_buffer(s_data.line_vertex_buffer);
		s_data.line_vertex_buffer_base = new LineVertex[s_data.max_vertices];

		s_data.white_texture = Texture2D::create(1, 1);
		uint32_t white_data = 0xffffffff;
		s_data.white_texture->set_data(&white_data, sizeof(uint32_t));
		
		int32_t samplers[s_data.max_texture_slots];
		for (int i = 0; i < s_data.max_texture_slots; i++)
			samplers[i] = i;

		s_data.texture_shader = Shader::create("Assets/shaders/shader.glsl");
		s_data.circle_shader = Shader::create("Assets/shaders/circle.glsl");
		s_data.line_shader = Shader::create("Assets/shaders/line.glsl");
		s_data.texture_shader->bind();
		s_data.texture_shader->set_int_array("u_textures", samplers, s_data.max_texture_slots);

		s_data.texture_slots[0] = s_data.white_texture;

		s_data.quad_vertex_positions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quad_vertex_positions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quad_vertex_positions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quad_vertex_positions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	}
	void Renderer2D::shutdown() {
	}

	void Renderer2D::begin_scene(const EditorCamera& camera) {
		RenderCommand::enable_depth();
		s_data.texture_shader->bind();
		s_data.texture_shader->set_mat4("u_view_projection", camera.get_view_projection());
		s_data.circle_shader->bind();
		s_data.circle_shader->set_mat4("u_view_projection", camera.get_view_projection());
		s_data.line_shader->bind();
		s_data.line_shader->set_mat4("u_view_projection", camera.get_view_projection());
		start_batch();
	}

	void Renderer2D::begin_scene(const OrthographicCamera& camera) {

		RenderCommand::enable_depth();
		s_data.texture_shader->bind();
		s_data.texture_shader->set_mat4("u_view_projection", camera.get_vp_matrix());
		s_data.circle_shader->bind();
		s_data.circle_shader->set_mat4("u_view_projection", camera.get_vp_matrix());
		s_data.line_shader->bind();
		s_data.line_shader->set_mat4("u_view_projection", camera.get_vp_matrix());
		start_batch();
	}

	void Renderer2D::begin_scene(const Camera& camera, const glm::mat4& transform) {
		RenderCommand::enable_depth();
		glm::mat4 view_projection = camera.get_projection() * glm::inverse(transform);

		s_data.texture_shader->bind();
		s_data.texture_shader->set_mat4("u_view_projection", view_projection);
		s_data.circle_shader->bind();
		s_data.circle_shader->set_mat4("u_view_projection", view_projection);
		s_data.line_shader->bind();
		s_data.line_shader->set_mat4("u_view_projection", view_projection);
		start_batch();
	}

	void Renderer2D::end_scene()
	{
		flush();
		if (s_data.overlay_circle || s_data.overlay_transform) {
			start_batch();
			RenderCommand::disable_depth();
			draw_overlay();
			flush();
			RenderCommand::enable_depth();
		}
	}

	void Renderer2D::flush() {
		if (s_data.quad_index_count) {
			uint32_t data_size = (uint8_t*)s_data.quad_vertex_buffer_ptr - (uint8_t*)s_data.quad_vertex_buffer_base;
			s_data.quad_vertex_buffer->set_data(s_data.quad_vertex_buffer_base, data_size);

			for (uint32_t i = 0; i < s_data.texture_slot_index; i++) {
				s_data.texture_slots[i]->bind(i);
			}

			s_data.texture_shader->bind();
			RenderCommand::draw_indexed(s_data.quad_vertex_array, s_data.quad_index_count);
			s_data.statistics.draw_calls++;
		}

		if (s_data.circle_index_count) {
			uint32_t data_size = (uint8_t*)s_data.circle_vertex_buffer_ptr - (uint8_t*)s_data.circle_vertex_buffer_base;
			s_data.circle_vertex_buffer->set_data(s_data.circle_vertex_buffer_base, data_size);

			s_data.circle_shader->bind();
			RenderCommand::draw_indexed(s_data.circle_vertex_array, s_data.circle_index_count);
			s_data.statistics.draw_calls++;
		}

		if (s_data.line_vertex_count) {
			uint32_t data_size = (uint8_t*)s_data.line_vertex_buffer_ptr - (uint8_t*)s_data.line_vertex_buffer_base;
			s_data.line_vertex_buffer->set_data(s_data.line_vertex_buffer_base, data_size);

			s_data.line_shader->bind();
			RenderCommand::set_line_width(s_data.line_width);
			RenderCommand::draw_lines(s_data.line_vertex_array, s_data.line_vertex_count);
			s_data.statistics.draw_calls++;
		}
	}

	void Renderer2D::start_batch() {
		s_data.quad_index_count = 0;
		s_data.quad_vertex_buffer_ptr = s_data.quad_vertex_buffer_base;

		s_data.circle_index_count = 0;
		s_data.circle_vertex_buffer_ptr = s_data.circle_vertex_buffer_base;

		s_data.line_vertex_count = 0;
		s_data.line_vertex_buffer_ptr = s_data.line_vertex_buffer_base;

		s_data.texture_slot_index = 1;
	}

	void Renderer2D::new_batch() {
		end_scene();
		start_batch();
	}

	void Renderer2D::draw_quad(const Quad& quad) {
		constexpr size_t quad_vertex_count = 4;
		if (s_data.quad_index_count >= Renderer2DStorage::max_indices)
			new_batch();
		float texture_index = 0.0f;
		std::array<glm::vec2, 4> texture_coords = { glm::vec2(0.0f, 0.0f), {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		if (quad.texture) {
			for (uint32_t i = 1; i < s_data.texture_slot_index; i++) {
				if (*s_data.texture_slots[i].get() == *quad.texture.get()) {
					texture_index = (float)i;
					break;
				}
			}

			if (texture_index == 0.0f) {
				texture_index = s_data.texture_slot_index;
				s_data.texture_slots[s_data.texture_slot_index] = quad.texture;
				s_data.texture_slot_index++;
			}

			if (quad.sub_texture)
				IV_WARN("Normal texture will shadow a sub-texture of spritesheet");
		}
		else if (quad.sub_texture) {
			texture_coords = quad.sub_texture->get_texture_coords();
			for (uint32_t i = 1; i < s_data.texture_slot_index; i++) {
				if (*s_data.texture_slots[i].get() == *quad.sub_texture->get_texture().get()) {
					texture_index = (float)i;
					break;
				}
			}

			if (texture_index == 0.0f) {
				texture_index = s_data.texture_slot_index;
				s_data.texture_slots[s_data.texture_slot_index] = quad.sub_texture->get_texture();
				s_data.texture_slot_index++;
			}
		}
		glm::mat4 transform;
		if (quad.transform == glm::mat4{ 1.0f })
			transform = glm::translate(glm::mat4(1.0f), quad.position) * glm::rotate(glm::mat4(1.0f), quad.rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { quad.size.x, quad.size.y, 0.0f });
		else
			transform = quad.transform;

		for(size_t i = 0; i < quad_vertex_count; i++) {
			s_data.quad_vertex_buffer_ptr->position = transform * s_data.quad_vertex_positions[i];
			s_data.quad_vertex_buffer_ptr->color = quad.color;
			s_data.quad_vertex_buffer_ptr->texture_coord = texture_coords[i];
			s_data.quad_vertex_buffer_ptr->texture_index = texture_index;
			s_data.quad_vertex_buffer_ptr->tiling_factor = quad.texture_info.tiling_factor;
			s_data.quad_vertex_buffer_ptr->entity_id = quad.entity_id;
			s_data.quad_vertex_buffer_ptr++;
		}

		s_data.quad_index_count += 6;

		s_data.statistics.quad_count++;
	}

	void Renderer2D::draw_circle(const Circle& circle) {
		// Do this for circles
		//if (s_data.quad_index_count >= Renderer2DStorage::max_indices)
		//	new_batch();

		for (size_t i = 0; i < 4; i++) {
			s_data.circle_vertex_buffer_ptr->world_position = circle.transform * s_data.quad_vertex_positions[i];
			s_data.circle_vertex_buffer_ptr->local_position = s_data.quad_vertex_positions[i] * 2.0f;
			s_data.circle_vertex_buffer_ptr->color = circle.color;
			s_data.circle_vertex_buffer_ptr->thickness = circle.thickness;
			s_data.circle_vertex_buffer_ptr->fade = circle.fade;
			s_data.circle_vertex_buffer_ptr->entity_id = circle.entity_id;
			s_data.circle_vertex_buffer_ptr++;
		}

		s_data.circle_index_count += 6;

		s_data.statistics.quad_count++;
	}

	void Renderer2D::draw_line(const glm::vec3& start_pos, const glm::vec3& end_pos, const glm::vec4& color, int entity_id) {
		s_data.line_vertex_buffer_ptr->position = start_pos;
		s_data.line_vertex_buffer_ptr->color = color;
		s_data.line_vertex_buffer_ptr->entity_id = entity_id;
		s_data.line_vertex_buffer_ptr++;

		s_data.line_vertex_buffer_ptr->position = end_pos;
		s_data.line_vertex_buffer_ptr->color = color;
		s_data.line_vertex_buffer_ptr->entity_id = entity_id;
		s_data.line_vertex_buffer_ptr++;

		s_data.line_vertex_count += 2;
	}

	void Renderer2D::draw_sprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id) {
		Quad quad{};
		quad.color = sprite.color;
		quad.transform = transform;
		quad.entity_id = entity_id;
		quad.texture = sprite.texture;
		quad.texture_info.tiling_factor = sprite.tiling_factor;
		draw_quad(quad);
	}

	void Renderer2D::draw_line_rectangle(const glm::mat4& transform, const glm::vec4& color, int entity_id) {
		glm::vec3 line_vertices[4];
		for (size_t i = 0; i < 4; i++)
			line_vertices[i] = transform * s_data.quad_vertex_positions[i];

		draw_line(line_vertices[0], line_vertices[1], color, entity_id);
		draw_line(line_vertices[1], line_vertices[2], color, entity_id);
		draw_line(line_vertices[2], line_vertices[3], color, entity_id);
		draw_line(line_vertices[3], line_vertices[0], color, entity_id);
	}
	void Renderer2D::draw_line_rectangle(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, int entity_id) {
		glm::vec3 p0 = glm::vec3{position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z};
		glm::vec3 p1 = glm::vec3{ position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z };
		glm::vec3 p2 = glm::vec3{ position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z };
		glm::vec3 p3 = glm::vec3{ position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z };

		draw_line(p0, p1, color, entity_id);
		draw_line(p1, p2, color, entity_id);
		draw_line(p2, p3, color, entity_id);
		draw_line(p3, p0, color, entity_id);
	}

	void Renderer2D::draw_overlay() {
		if(s_data.overlay_transform) Renderer2D::draw_line_rectangle(*s_data.overlay_transform, glm::vec4{ 0.9f, 0.9f, 0.7f, 1.0f }, s_data.overlay_entity_id);
		if (s_data.overlay_circle) {
			Circle circ = *s_data.overlay_circle;
			circ.fade = 0.01;
			circ.thickness = 0.01;
			circ.color = glm::vec4{ 0.9f, 0.9f, 0.7f, 1.0f };
			Renderer2D::draw_circle(circ);
		}

	}

	void Renderer2D::draw_spring(const glm::vec3& pos_a, const glm::vec3& pos_b, float height, int revolutions, float rest_length, int entity_id) {
		//draw_line(pos_a, pos_b, glm::vec4(1.0f), entity_id);
		/*glm::vec3 point_to_point_vec = (pos_b - pos_a) / (float)revolutions;
		float p_to_p_length = sqrtf(point_to_point_vec.x * point_to_point_vec.x + point_to_point_vec.y * point_to_point_vec.y) / (float)revolutions;
		float normal_p_to_p_length = rest_length * 100 / revolutions;
		std::cout << "PtoP" << p_to_p_length << "normal" << normal_p_to_p_length;
		glm::vec3 last_pos = pos_a;
		glm::vec3 cross_vec = glm::cross(point_to_point_vec, glm::vec3(0.0, 0.0, 1.0));
		float new_height = sqrtf(normal_p_to_p_length * normal_p_to_p_length - p_to_p_length * p_to_p_length);
		cross_vec /= (float)cross_vec.length();
		cross_vec *= new_height ;
		std::cout << cross_vec.length();
		for (int i = 0; i < revolutions; i++) {
			//glm::vec3 second_pos = pos_a + (float)(i + 1) * point_to_point_vec;
			//float new_height =  ? height : -height;
			//second_pos = second_pos + glm::vec3()
			
			glm::vec3 second_pos = (last_pos + point_to_point_vec) + (cross_vec / (float)cross_vec.length()) * ((i % 2 == 0) ? 1.0f : -1.0f);
			draw_line(last_pos, second_pos, glm::vec4(1.0f), entity_id);
			last_pos = second_pos;
		}*/
		float opp = 5.0f / revolutions;
		glm::vec3 npp_vec = (pos_b - pos_a) / (float)revolutions;
		float npp_len = sqrtf(npp_vec.x * npp_vec.x + npp_vec.y * npp_vec.y) / (float)revolutions;
		float n_height = sqrtf(opp * opp - npp_len * npp_len) * height;

		glm::vec3 cross_vec = glm::cross(npp_vec, glm::vec3(0.0, 0.0, 1.0));
		cross_vec /= sqrtf(cross_vec.x * cross_vec.x + cross_vec.y * cross_vec.y);
		glm::vec3 last_pos = pos_a;

		for (int i = 0; i < revolutions; i++) {
			//glm::vec3 second_pos = pos_a + (float)(i + 1) * point_to_point_vec;
			//float new_height =  ? height : -height;
			//second_pos = second_pos + glm::vec3()
			
			glm::vec3 second_pos = (last_pos + npp_vec) + (cross_vec * n_height) * ((i % 2 == 0) ? 1.0f : -1.0f);
			draw_line(last_pos, second_pos, glm::vec4(1.0f), entity_id);
			last_pos = second_pos;
		}
		
	}

	void Renderer2D::set_line_width(float width) {
		s_data.line_width = width;
	}
	float Renderer2D::get_line_width() {
		return s_data.line_width;
	}

	void Renderer2D::reset_stats() { memset(&s_data.statistics, 0, sizeof(s_data.statistics)); }
	const Renderer2D::Statistics& Renderer2D::get_stats() { return s_data.statistics; }
}