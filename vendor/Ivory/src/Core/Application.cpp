#include "pch.h"
#include "Application.h"
#include "Events/WindowEvent.h"
#include "Events/KeyEvent.h"
#include "Input.h"
#include "Rendering/Renderer.h"
#include "Platform/Platform.h"

namespace Ivory {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_instance = nullptr;

	

	Application::Application(const std::string& name) {
		// Only one application can run at a time
		IV_CORE_ASSERT(!s_instance, "Application already exists");
		s_instance = this;
		// Create window on application start
		WindowProps props{};
		props.title = name;
		m_window = std::unique_ptr<Window>(Window::create(props));
		m_window->set_event_callback(BIND_EVENT_FN(on_event));

		Renderer::init();

		m_imgui_layer = std::make_shared<ImGuiLayer>();
		push_overlay(m_imgui_layer);

		
	}

	void Application::on_event(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(on_window_close));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(on_window_resize));

		// Traverse layer stack in reverse and let a layer handle it.
		// If the event is handled by the layer then stop going deeper, else let the next layer handle the event.
		for (auto it = m_layer_stack.end(); it != m_layer_stack.begin(); ) {
			(*--it)->on_event(e);
			if (e.is_handled())
				break;
		}
	}

	void Application::run() {
		while (m_running) {
			float time = Platform::get_time();
			Timestep timestep = time - m_last_frame_time;
			m_last_frame_time = time;

			if (!m_minimized)
				// Update every layer
				for (shared_ptr<Layer>& layer : m_layer_stack)
					layer->on_update(timestep);

			// Start imgui frame
			m_imgui_layer->begin();

			for (shared_ptr<Layer>& layer : m_layer_stack)
				layer->on_imgui_render();
			m_imgui_layer->end();

			m_window->on_update();
		}
	}

	bool Application::on_window_close(WindowCloseEvent& e) {
		m_running = false;
		return true;
	}

	bool Application::on_window_resize(WindowResizeEvent& e) {
		if (e.get_width() == 0 || e.get_height() == 0) {
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::on_window_resize(e.get_width(), e.get_height());

		return true;
	}

	void Application::push_layer(shared_ptr<Layer> layer) {
		m_layer_stack.push_layer(layer);
		layer->on_attach();
	}

	void Application::push_overlay(shared_ptr<Layer> overlay) {
		m_layer_stack.push_layer(overlay);
		overlay->on_attach();
	}
}