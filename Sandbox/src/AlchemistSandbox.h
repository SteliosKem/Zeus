#pragma once
#include <Core/IvoryEngine.h>
#include "PointMass.h"

class AlchemistSandbox : public Ivory::Layer {
public:
	AlchemistSandbox();

	void on_attach() override;
	void on_detach() override;

	void on_update(Ivory::Timestep dt) override;
	void on_imgui_render() override;

	void on_event(Ivory::Event& e) override;
private:
	std::shared_ptr<Ivory::Shader> m_shader;
	std::shared_ptr<Ivory::VertexArray> m_vertex_array;
	

	std::shared_ptr<Ivory::SubTexture2D> m_sprite;

	Ivory::OrthographicCameraController m_camera_controller;
	glm::vec3 m_color{ 1.0f };

	std::unordered_map<char, std::shared_ptr<Ivory::SubTexture2D>> m_subtexture_map;


	Alchemist::PointMass2D m_physics_obj;
	Alchemist::PointMass2D m_spring_anchor;
	Alchemist::ForceRegistry m_force_registry;
	Alchemist::PointMassGravity* m_gravity;
	Alchemist::PointMassGravity* m_gravity2;
	Alchemist::PointMassSpring* m_spring;
	Alchemist::AnchoredPointMassSpring* m_spring2;
};