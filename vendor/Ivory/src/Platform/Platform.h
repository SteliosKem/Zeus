#pragma once
// TO CHANGE ***********
#include <GLFW/glfw3.h>

namespace Ivory {
	class Platform {
	public:
		static float get_time() {
			return glfwGetTime();
		}
	};
}