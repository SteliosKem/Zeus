#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "Rendering/Texture.h"
#include <memory>
#include "Scene/Entity.h"

namespace Ivory {
	enum class Quantity {
		HorizontalPosition,
		VerticalPosition,
		Velocity,
		HorizontalVelocity,
		VerticalVelocity,
		Acceleration,
		HorizontalAcceleration,
		VerticalAcceleration
	};

	class Grapher {
	public:
		Grapher() { m_knot_icon = Texture2D::create("Assets/EditorIcons/knot.png"); }
		void on_imgui_render(bool has_recording, int32_t time);

		void start(std::shared_ptr<Scene> scene) { m_scene = scene; }
		void add(Entity entity, const std::string& name, Quantity quantity);
		void remove(const std::string& name);
		void insert(const std::string& name, float value);
		void update();
		void reset();
		void clear_lists();
	private:
		struct WatchInfo {
			Entity entity;
			Quantity quantity;
		};
		std::unordered_map<std::string, WatchInfo> m_watch_list;
		std::unordered_map<std::string, std::vector<float>> m_map;
		std::shared_ptr<Texture2D> m_knot_icon;
		std::shared_ptr<Scene> m_scene;
	};
}