#include "pch.h"
#include "WinPlatformWindow.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Rendering/GraphicsContext.h"
#include <Platform/OpenGL/OpenGLContext.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Ivory {
	// Check if glfw is already up
	static bool s_GLFWInitialized = false;

	static void GLFW_error_callback(int error, const char* desc) {
		IV_CORE_ERROR("GLFW Error ({0}): {1}", error, desc);
	}

	Window* Window::create(const WindowProps& props) { return new WinPlatformWindow(props); }
	WinPlatformWindow::WinPlatformWindow(const WindowProps& props) { init(props); }
	WinPlatformWindow::~WinPlatformWindow() { shutdown(); }

	void WinPlatformWindow::init(const WindowProps& props) {
		// Window setup
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		

		IV_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!s_GLFWInitialized) {
			// Initialize glfw
			int success = glfwInit();
			IV_CORE_ASSERT(success, "Could not initialize GLFW");
			glfwSetErrorCallback(GLFW_error_callback);
			s_GLFWInitialized = true;
		}

		// Create window
		m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
		m_context = std::make_shared<OpenGLContext>(m_window);
		m_context->init();

		glfwSetWindowUserPointer(m_window, &m_data);
		set_vsync(true);

		// GLFW event callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;
			
			WindowResizeEvent event(width, height);
			data.event_callback(event);
			
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			data.event_callback(e);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent e(key, 0);
				data.event_callback(e);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent e(key);
				data.event_callback(e);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent e(key, 1);
				data.event_callback(e);
				break;
			}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent e(character);
			data.event_callback(e);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent e(button);
				data.event_callback(e);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent e(button);
				data.event_callback(e);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x_offset, double y_offset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			MouseScrollEvent e((float)x_offset, (float)y_offset);
			data.event_callback(e);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x_pos, double y_pos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x_pos, (float)y_pos);
			data.event_callback(e);
		});

		glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowResizeEvent e(0.0f, 0.0f);
			data.event_callback(e);
		});
	}

	void WinPlatformWindow::shutdown() {
		glfwDestroyWindow(m_window);
	}

	void WinPlatformWindow::on_update() {
		glfwPollEvents();
		m_context->swap_buffers();
	}

	void WinPlatformWindow::set_vsync(bool enabled) {
		enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
		m_data.vsync = enabled;
	}

	void WinPlatformWindow::set_title(const std::string& title) {
		m_data.title = title;
		glfwSetWindowTitle(m_window, title.c_str());
	}

	bool WinPlatformWindow::get_vsync() const { return m_data.vsync; }

	void WinPlatformWindow::set_image(const std::string& path) {
		GLFWimage images[1];
		images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(m_window, 1, images);
		stbi_image_free(images[0].pixels);
	}
}