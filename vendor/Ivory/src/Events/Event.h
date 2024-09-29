#pragma once

#include "pch.h"
#include "Core/Core.h"

namespace Ivory {
	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

	// Macro that implements standard functions related to the event type
#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::##type; }\
	virtual EventType get_event_type() const override { return get_static_type(); }\
	virtual const char* get_name() const override { return #type; }

	// Macro that implements standard function related to the event category
#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

	class IVORY_API Event {
		friend class EventDispatcher;
	public:
		virtual EventType get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int get_category_flags() const = 0;
		virtual std::string to_string() const { return get_name(); }

		inline bool is_in_category(EventCategory category) {
			return get_category_flags() & category;
		}

		inline bool is_handled() { return m_handled; }
		inline void handled(bool is) { m_handled = is; }

	protected:
		bool m_handled = false;
	};

	// The event dispatcher handles calling callbacks depending on the event type
	class EventDispatcher {
		template<typename T>
		using EventFunction = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& _event) : m_event(_event) {}

		template<typename T>
		bool dispatch(EventFunction<T> function) {
			if (m_event.get_event_type() == T::get_static_type()) {
				m_event.m_handled = function(*(T*)&m_event);
				return true;
			}
			return false;
		}
	private:
		Event& m_event;
	};
	
}
