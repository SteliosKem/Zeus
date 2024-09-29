#include "pch.h"
#include "Core/Input.h"

#include "Core/Application.h"
#include "GLFW/glfw3.h"

namespace Ivory {
	bool Input::is_mouse_button_pressed(int keycode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native());
		int state = glfwGetMouseButton(window, keycode);
		return state == GLFW_PRESS;
	}

	bool Input::is_key_pressed(int keycode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native());
		int state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	glm::vec2 Input::mouse_pos() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return glm::vec2{ (float)x, (float)y };
	}
}