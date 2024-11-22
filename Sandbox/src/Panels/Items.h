#pragma once
#include "Rendering/Texture.h"
#include "Scene/Components.h"

namespace Ivory {
	class Items {
	public:
		Items() {
			m_rope_tex = Texture2D::create("Assets/EditorIcons/rope.png");
			m_circle_tex = Texture2D::create("Assets/EditorIcons/circle.png");
			m_square_tex = Texture2D::create("Assets/EditorIcons/square.png");
			m_floor_tex = Texture2D::create("Assets/EditorIcons/floor.png");
			m_wall_tex = Texture2D::create("Assets/EditorIcons/wall.png");
			m_spring_tex = Texture2D::create("Assets/EditorIcons/spring.png");
		}
		void on_imgui_render();
	private:
		std::shared_ptr<Texture2D> m_rope_tex;
		std::shared_ptr<Texture2D> m_circle_tex;
		std::shared_ptr<Texture2D> m_square_tex;
		std::shared_ptr<Texture2D> m_floor_tex;
		std::shared_ptr<Texture2D> m_wall_tex;
		std::shared_ptr<Texture2D> m_spring_tex;
		PhysicsObjectType m_point_mass_square = PointMassSquare;
		PhysicsObjectType m_point_mass_circle = PointMassCircle;
		PhysicsObjectType m_wall = Wall;
		PhysicsObjectType m_floor = Floor;
		PhysicsObjectType m_rope = Rope;
		PhysicsObjectType m_rod = Rod;
		PhysicsObjectType m_spring = Spring;
	};
}