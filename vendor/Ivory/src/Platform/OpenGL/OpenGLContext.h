#pragma once
#include "Rendering/GraphicsContext.h"

struct GLFWwindow;

namespace Ivory {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void init() override;
		virtual void swap_buffers() override;
	private:
		GLFWwindow* m_window;
	};
}