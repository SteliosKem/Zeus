#pragma once

namespace Ivory {
	class Preferences {
	public:
		Preferences() = default;
		void load();
		void on_imgui_render();
		void set_time_per_frame(float time) { m_original_time = time; m_time_per_frame = time; }
		float get_time_per_frame() { return m_original_time; }
	private:
		void save();
		float m_original_time;
		float m_time_per_frame;
	};
}