#include "Timeline.h"
#include "ImGui/Sequencer/imgui_neo_sequencer.h"

namespace Zeus {
	void Timeline::on_imgui_render() {
		int32_t current_frame = m_current_time;
		int32_t start_frame = 0;
		int32_t end_frame = (m_is_playing) ? m_current_time : m_time_played;
		static bool a = false;
		std::vector<int32_t> collisions;

		if (ImGui::BeginNeoSequencer("Timeline", (m_is_playing) ? &current_frame : &m_current_time, & start_frame, &end_frame)) {
			ImGui::BeginNeoTimeline("Collisions", collisions);
			if(m_scene)
				for (auto& collision : m_scene->get_collisions()) {
					ImGui::NeoKeyframe((int32_t*) & collision.frame);
				}
			
			ImGui::EndNeoTimeLine();
			ImGui::EndNeoSequencer();
		}
	}
}