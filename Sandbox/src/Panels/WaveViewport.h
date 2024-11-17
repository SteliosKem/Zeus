#pragma once
#include "imgui.h"
#include "ImGui/ImPlot/implot.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace Ivory {
	class WaveViewport {
	public:
		struct Point {
			float height;
			float velocity;
			float acceleration;
		};

		struct Wave {
			float velocity;
			float wave_length;
			float frequency;
			float amplitude;
			std::vector<float> height_points;
		};

		void on_imgui_render();
		void create_wave(const std::string& name, float velocity, float wave_length, float frequency, float amplitude);
		void calculate_wave(float time);
		void on_update(float dt) { m_time += dt; calculate_wave(m_time); }
		void reset() { m_time = 0; for(auto& [name, wave] : m_waves) wave.height_points.clear(); }
	private:
		float m_time;

		std::unordered_map<std::string, Wave> m_waves;
		std::unordered_map<std::string, float> m_watch_points;
	};
}