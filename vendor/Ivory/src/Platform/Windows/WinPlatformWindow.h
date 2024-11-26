#pragma once
#include "Core/Window.h"

#include "GLFW/glfw3.h"
#include "Rendering/GraphicsContext.h"

namespace Ivory {
	class WinPlatformWindow : public Window {
	public:
		WinPlatformWindow(const WindowProps& props);
		virtual ~WinPlatformWindow();

		void on_update() override;

		inline unsigned int get_width() const override { return m_data.width; }
		inline unsigned int get_height() const override { return m_data.height; }

		inline void set_event_callback(const EventCallback& callback) override { m_data.event_callback = callback; }
		void set_vsync(bool enabled);
		bool get_vsync() const;
		void set_title(const std::string& title) override;

		void set_image(const std::string& path) override;

		inline virtual void* get_native() const { return m_window; };
	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();
		std::shared_ptr<GraphicsContext> m_context;

		GLFWwindow* m_window;

		struct WindowData {
			std::string title;
			unsigned int width, height;
			bool vsync;
			EventCallback event_callback;		// Callback to call on every event
		};

		WindowData m_data;
	};
}