#pragma once
#include <glm/glm.hpp>

namespace Ivory {
	class IVORY_API Input {
	public:
		static bool is_key_pressed(int keycode);
		static bool is_mouse_button_pressed(int keycode);
		static glm::vec2 mouse_pos();
	};
}