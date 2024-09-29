#include "AlchemistSandbox.h"
#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

AlchemistSandbox::AlchemistSandbox() : Layer("Test2D"), m_camera_controller(1280.0f / 720.f), m_physics_obj{ 1.0f }, m_spring_anchor{ 1.0f } {}

void AlchemistSandbox::on_attach() {
	std::cout << 0.1f + 0.9f;
	m_gravity = new Alchemist::PointMassGravity({0.0f, -2.0f});
	m_gravity2 = new Alchemist::PointMassGravity({ 0.8f, 0.0f });
	//m_physics_obj.set_position({ 0.4f, 0.0f });
	//m_spring_anchor.set_acceleration({ 0.5f, 0.0f });
	m_physics_obj.set_damp(0.6f);
	m_spring = new Alchemist::PointMassSpring(&m_spring_anchor, 20, 1);
	m_spring2 = new Alchemist::AnchoredPointMassSpring(glm::vec2{2.0f, 0.0f}, 20, 1);
	m_force_registry.add(&m_physics_obj, m_gravity);
	//m_force_registry.add(&m_spring_anchor, m_gravity2);
	m_force_registry.add(&m_physics_obj, m_spring);
	m_force_registry.add(&m_physics_obj, m_spring2);
}

void AlchemistSandbox::on_detach() {
	delete m_gravity;
	delete m_spring;
}

void AlchemistSandbox::on_update(Ivory::Timestep dt) {
	
	
	m_force_registry.update_forces(dt);
	m_camera_controller.on_update(dt);
	m_spring_anchor.on_update(dt);
	m_physics_obj.on_update(dt);

	Ivory::RenderCommand::set_clear_color({ 0.1f, 0.1f, 0.1f, 1 });
	Ivory::RenderCommand::clear();

	Ivory::Renderer2D::begin_scene(m_camera_controller.get_camera());

	Ivory::Quad quad2{};
	quad2.color = glm::vec4(m_color, 1.0f);
	quad2.size = {0.1f, 0.1f};
	quad2.position = { m_physics_obj.get_position(), 0.0f };

	Ivory::Renderer2D::draw_quad(quad2);

	quad2.color = glm::vec4(m_color, 0.5f);
	quad2.size = { 0.04f, 0.04f};
	quad2.position = { m_spring_anchor.get_position(), 0.0f };

	Ivory::Renderer2D::draw_quad(quad2);

	quad2.color = glm::vec4(m_color, 0.5f);
	quad2.size = { 0.04f, 0.04f };
	quad2.position = { 2.0f, 0.0f, 0.0f };

	Ivory::Renderer2D::draw_quad(quad2);

	Ivory::Renderer2D::end_scene();
}

void AlchemistSandbox::on_imgui_render() {
	ImGui::Begin("Scene Settings");
	ImGui::Text("Object");
	ImGui::DragFloat2("Position", glm::value_ptr(m_physics_obj.get_position()));
	ImGui::DragFloat2("Velocity", glm::value_ptr(m_physics_obj.get_velocity()));
	ImGui::DragFloat2("Acceleration", glm::value_ptr(m_physics_obj.get_acceleration()));

	ImGui::Text("Spring Anchor");
	ImGui::DragFloat2("Anchor Position", glm::value_ptr(m_spring_anchor.get_position()));
	ImGui::DragFloat2("Anchor Velocity", glm::value_ptr(m_spring_anchor.get_velocity()));
	ImGui::DragFloat2("Anchor Acceleration", glm::value_ptr(m_spring_anchor.get_acceleration()));
	ImGui::Separator();
	ImGui::DragFloat("Spring Constant", &m_spring->get_constant());
	ImGui::DragFloat("Spring Length", &m_spring->get_rest_length());
	ImGui::End();
}

void AlchemistSandbox::on_event(Ivory::Event& e) {
	m_camera_controller.on_event(e);
}