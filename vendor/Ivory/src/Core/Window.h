#pragma once
#include "pch.h"

#include "Core.h"
#include "Events/Event.h"

namespace Ivory {
	// Window data
	struct WindowProps {
		std::string title = "Ivory Engine";
		uint32_t width = 1280;
		uint32_t height = 720;
	};

	class IVORY_API Window {
	public:
		// Callback function type
		using EventCallback = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void on_update() = 0;

		virtual uint32_t get_width() const = 0;
		virtual uint32_t get_height() const = 0;

		virtual void set_event_callback(const EventCallback& func) = 0;
		virtual void set_vsync(bool enable) = 0;
		virtual bool get_vsync() const = 0;

		virtual void* get_native() const = 0;
		virtual void set_image(const std::string& path) = 0;

		static Window* create(const WindowProps& props = WindowProps{});
	};
}