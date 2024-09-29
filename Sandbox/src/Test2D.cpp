#include "Test2D.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

const uint32_t map_width = 30;
const uint32_t map_height = 14;
static const char* tiles =
"CCCCCCDDDDDDDDDDCCCCCCCCCCCCCC"
"DDDDDDDDDDDDDDDDDDDDDRDDDDDDDD"
"DDDDDDRDDDDDDDDDDDDDDDDDDDDDDD"
"DDDDDDDDDDRDDDDDDDDDDDDDDDDDDD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
"DDDDDCCCCCCCDDDDDRDDDDDDDDDDDD"
"DDDDDCCCCCCCDDDDDDDDDDDDDDDDDD"
"DDDDDDDDDDDDDRDDDDCCCCCCDDDDDD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
"DDDDDRDDDDDDDDDDDDCCCCCCDDDDDD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"
"DDDDDDDDDDRDDDDDDDDDDDDDDDDDDD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDRDDD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDD";

Test2D::Test2D() : Layer("Test2D"), m_camera_controller(1280.0f / 720.f) {

}

void Test2D::on_attach() {
	m_texture = Ivory::Texture2D::create("C:/Projects/Ivory-Engine/Editor/Assets/Zeus.png");
	m_texture2 = Ivory::Texture2D::create("C:/Projects/Ivory-Engine/Editor/Assets/IVlogo.png");
	m_sprite_sheet = Ivory::Texture2D::create("C:/Projects/Ivory-Engine/Editor/Assets/tilemap.png");

	m_subtexture_map['D'] = Ivory::SubTexture2D::create_from_coords(m_sprite_sheet, {0, 6}, {16, 16}, {1,1}, 1);
	m_subtexture_map['R'] = Ivory::SubTexture2D::create_from_coords(m_sprite_sheet, { 1, 6 }, { 16, 16 }, { 1,1 }, 1);
	m_subtexture_map['C'] = Ivory::SubTexture2D::create_from_coords(m_sprite_sheet, { 4, 7 }, { 16, 16 }, { 1,1 }, 1);
}
void Test2D::on_detach() {}

void Test2D::on_update(Ivory::Timestep dt) {
	static float rot = 0;
	rot += dt;
	float x_pos = 2 * sinf(rot);
	m_camera_controller.on_update(dt);

	Ivory::Renderer2D::reset_stats();
	if (Ivory::Input::is_mouse_button_pressed(2)) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	else {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	}

	Ivory::RenderCommand::set_clear_color({ 0.1f, 0.1f, 0.1f, 1 });
	Ivory::RenderCommand::clear();

	Ivory::Renderer2D::begin_scene(m_camera_controller.get_camera());

	Ivory::Quad textured_quad{ {1.1f, 1.1f, 0.0f }, { 0.8f, 0.8f }, 0, {0.4f, 1.0f, 1.0f, 1.0f}, m_texture };
	Ivory::Quad textured_quad2{ {0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f }, rot, {1.0f, 1.0f, 1.0f, 1.0f}, m_texture2 };
	textured_quad.texture_info.tiling_factor = 4.0f;
	Ivory::Quad quad2{};
	Ivory::Quad quad3{ {1.0f, 0, 0}, {1.0f, 1.0f}, 0, {0.5f, 0.2f, 0.2f, 1.0f} };
	quad2.color = glm::vec4(m_color, 1.0f);

	Ivory::Renderer2D::draw_quad(quad2);
	Ivory::Renderer2D::draw_quad(quad3);
	Ivory::Renderer2D::draw_quad(textured_quad2);
	Ivory::Renderer2D::draw_quad(textured_quad);
	Ivory::Renderer2D::end_scene();
	Ivory::Renderer2D::begin_scene(m_camera_controller.get_camera());
	for (float y = -5.0f; y < 5.0f; y += 0.25f) {
		for (float x = -0.5f; x < 5.0f; x += 0.25f) {
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
			Ivory::Renderer2D::draw_quad({ {x + x_pos, y, 0}, {0.2f, 0.2f}, rot / 5, color, nullptr });
		}
	}
	//Ivory::Renderer2D::draw_quad({ 1.1f, 1.1f, 0.0f }, { 0.8f, 0.8f }, m_texture2);
	//Ivory::Renderer2D::draw_quad({ -1.1f, 1.1f, 0.0f }, { 2.0f, 2.0f }, m_texture);
	
	//Ivory::Renderer2D::end_scene();
	Ivory::Renderer2D::end_scene();

	Ivory::Renderer2D::begin_scene(m_camera_controller.get_camera());
	Ivory::Renderer2D::draw_quad({ {10.0f, 0.0f, 0.0f}, {2.0f, 2.0f}, 0, {1.0f, 1.0f, 1.0f, 1.0f}, m_sprite_sheet});
	Ivory::Renderer2D::draw_quad({ {10.0f, 2.0f, 0.0f}, {2.0f, 1.0f}, 0, {1.0f, 1.0f, 1.0f, 1.0f}, nullptr, m_sprite });
	Ivory::Renderer2D::end_scene();

	Ivory::Renderer2D::begin_scene(m_camera_controller.get_camera());
	for (uint32_t y = 0; y < map_height; y++) {
		for (uint32_t x = 0; x < map_width; x++) {
			char type = tiles[x + y * map_width];
			Ivory::Renderer2D::draw_quad({ {10.0f + x, 4.0f + y, 0.0f}, {1.0f, 1.0f}, 0, {1.0f, 1.0f, 1.0f, 1.0f}, nullptr, m_subtexture_map[type]});
		}
		
	}
	
	Ivory::Renderer2D::end_scene();
}

void Test2D::on_imgui_render() {
	ImGui::Begin("Settings");
	ImGui::ColorPicker3("Color", glm::value_ptr(m_color));

	auto stats = Ivory::Renderer2D::get_stats();

	ImGui::Text("Renderer Stats:");
	ImGui::Text("Draw Calls: %d", stats.draw_calls);
	ImGui::Text("Quads: %d", stats.quad_count);
	ImGui::Text("Vertices: %d", stats.get_vertex_count());
	ImGui::Text("Indices: %d", stats.get_index_count());
	ImGui::End();
}

void Test2D::on_event(Ivory::Event& e) {
	m_camera_controller.on_event(e);
}