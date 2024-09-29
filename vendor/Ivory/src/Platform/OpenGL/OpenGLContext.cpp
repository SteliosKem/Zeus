#include "pch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>

#include "glad/glad.h"

namespace Ivory {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : m_window(window) {

	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_window);
		// Load glad
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		IV_CORE_ASSERT(status, "Failed to initialize GLAD");

		IV_CORE_INFO("OpenGL Core Renderer: {0} {1}", (const char*)glGetString(GL_VENDOR), (const char*)glGetString(GL_RENDERER));
	}

	void OpenGLContext::swap_buffers() {
		glfwSwapBuffers(m_window);
	}
}