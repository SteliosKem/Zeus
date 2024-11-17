#include "WaveViewport.h"
#include "UI.h"

namespace Ivory {
	void WaveViewport::on_imgui_render() {
		ImGui::Begin("Wave Viewport");
		if (ImGui::Button("Add Wave")) {
			m_waves[std::string("Wave ") + std::to_string(m_waves.size())] = Wave{ 1.0f, 1.0f, 1.0f, 1.0f };
		}
		for (auto& [name, wave] : m_waves) {
			if (ImGui::CollapsingHeader(name.c_str())) {
				draw_label(("Wave Length##" + name).c_str(), wave.wave_length, 0.05f);
				draw_label(("Frequency##" + name).c_str(), wave.frequency, 0.05f);
				draw_label(("Amplitude##" + name).c_str(), wave.amplitude, 0.05f);
			}
		}

		if (ImGui::Button("Add Point")) {
			m_watch_points[std::string("Point ") + std::to_string(m_watch_points.size())] = 0.0f;
		}
		for (auto& [name, x] : m_watch_points) {
			ImGui::DragFloat(("Position##" + name).c_str(), &(float)x);
			if (ImGui::Button(("X##" + name).c_str()))
				m_watch_points.erase(name);
		}

		if (ImPlot::BeginPlot("Waves", { ImGui::GetContentRegionAvail() })) {
			for(auto& [name, wave] : m_waves)
				if(!wave.height_points.empty())
					ImPlot::PlotLine(name.c_str(), &wave.height_points[0], wave.height_points.size());
			

			ImPlot::EndPlot();
		}
		ImGui::End();
	}

	void WaveViewport::create_wave(const std::string& name, float velocity, float wave_length, float frequency, float amplitude) {
		m_waves["name"] = Wave{velocity, wave_length, frequency, amplitude};
	}

	void WaveViewport::calculate_wave(float time) {
		for (auto& [name, wave] : m_waves) {
			wave.velocity = wave.frequency * wave.wave_length;
			wave.height_points.clear();
			float x = wave.velocity * time;
			float x_points = 100;
			for (int i = 0; i < x * 100; i++) {
				wave.height_points.push_back(wave.amplitude * sinf(wave.frequency * time - (wave.velocity > 0 ? 1 : -1) * (i / 100.0f) / wave.wave_length));
			}
		}
	}
}