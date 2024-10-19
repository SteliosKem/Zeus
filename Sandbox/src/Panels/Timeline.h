#pragma once
#include <memory>
#include <vector>
#include "Scene/Scene.h"

namespace Zeus {
	class Timeline {
	public:
		Timeline() = default;
		void on_imgui_render();

		float get_current_time() { return m_current_time; }

		void increment(float dt) { m_current_time += 1; m_time_played = m_current_time; }
		void reset() { m_time_played = m_current_time; m_current_time = 0;  }
		void set_is_playing(bool is) { m_is_playing = is; }
	private:
		int32_t m_current_time = 0;
		int32_t m_time_played = 1;
		bool m_is_playing = false;
	};
}