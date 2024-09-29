 #pragma once
#include "Layers/Layer.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/WindowEvent.h"

namespace Ivory {
	class IVORY_API ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_imgui_render() override;
		void on_event(Event& e) override;
		void begin();
		void end();

		void set_block_events(bool set) { m_block_events = set; }
	private:
		bool m_block_events = true;
		float m_time = 0.0f;
	};
}