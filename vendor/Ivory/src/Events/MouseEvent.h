#pragma once

#include "Event.h"

namespace Ivory {
	class IVORY_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y) : m_mouse_x(x), m_mouse_y(y) {}

		inline float get_x() const { return m_mouse_x; }
		inline float get_y() const { return m_mouse_y; }

		std::string to_string() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << m_mouse_x << ", " << m_mouse_y << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_mouse_x, m_mouse_y;
	};

	class IVORY_API MouseScrollEvent : public Event {
	public:
		MouseScrollEvent(float x, float y) : m_offset_x(x), m_offset_y(y) {}

		inline float get_offset_x() const { return m_offset_x; }
		inline float get_offset_y() const { return m_offset_y; }

		std::string to_string() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: (" << m_offset_x << ", " << m_offset_y << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_offset_x, m_offset_y;
	};

	class IVORY_API MouseButtonEvent : public Event {
	public:
		inline int get_mouse_button() const { return m_mouse_button; }
		
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(int button) : m_mouse_button(button) {}
		int m_mouse_button;
	};

	class IVORY_API MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string to_string() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_mouse_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class IVORY_API MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string to_string() const override {
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_mouse_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}