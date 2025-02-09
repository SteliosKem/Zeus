#pragma once
#include <Core/IvoryEngine.h>

#include "Core/Entry.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "ImGui/ImGuiNotify.h"

#include "Test2D.h"

#include "Panels/SceneHierarchy.h"
#include "Panels/ContentBrowser.h"
#include "Panels/WorldSettings.h"
#include "Panels/Timeline.h"

#include "Windows/ProjectSetupWindow.h"

#include "PointMass.h"
#include "Snapshot.h"
#include "Panels/Grapher.h"
#include "Panels/WaveViewport.h"
#include "Panels/Items.h"
#include "Panels/Preferences.h"

#include "Action.h"


const std::string icon_path = "Assets/Zeus.png";
const std::string shader_path = "Assets/shaders/shader.glsl";

namespace Zeus {
	using namespace Ivory;

	class EditorLayer : public Ivory::Layer {
	public:
		EditorLayer();

		void on_attach() override;
		void on_detach() override;

		void on_update(Ivory::Timestep dt) override;
		void on_imgui_render() override;

		void on_event(Ivory::Event& e) override;
		void show_setup_window() {}
	private:
		bool on_key_pressed(KeyPressedEvent& e);
		bool on_mouse_button_pressed(MouseButtonPressedEvent& e);
		bool on_mouse_button_released(MouseButtonReleasedEvent& e);

		void on_deselect();

		void open_scene();
		void open_scene(const std::filesystem::path& path);
		void save_scene();
		void save_scene_as();
		void new_scene(Scene::SceneType scene_type = Scene::PointMasses);

		void new_project(const std::filesystem::path& path, const std::string& name = "");
		void open_project();
		void open_project(const std::filesystem::path& path);
		void save_project(bool save_as);
		void save_project(const std::filesystem::path& path);

		void on_scene_record();
		void on_scene_pause();
		void on_scene_simulate();
		void on_scene_play();
		void on_scene_stop();
		void clear_recording();
		void delete_entity(Entity entity);

		glm::vec2 get_world_pos_from_mouse(); // Check now for mouse coordinates
		glm::vec2 get_world_pos_from_mouse(const glm::vec2& mouse_coords);

		void on_duplicate_entity();

		void ui_toolbar();


		// Editor Debugging Tools
		enum class LogType {
			Info,
			Warn,
			Error
		};

		void log_and_notify(const std::string& message, LogType type);

		enum class SceneState {
			Edit,
			Play,
			Paused,
			Simulate,
			Recording
		};

		SceneState m_scene_state = SceneState::Edit;

		std::shared_ptr<Ivory::Texture2D> m_play_icon;
		std::shared_ptr<Ivory::Texture2D> m_record_icon;
		std::shared_ptr<Ivory::Texture2D> m_simulate_icon;
		std::shared_ptr<Ivory::Texture2D> m_stop_icon;
		std::shared_ptr<Ivory::Texture2D> m_pause_icon;

		std::shared_ptr<Ivory::Scene> m_active_scene, m_editor_scene;

		
		EditorCamera m_editor_camera; 

		Entity m_camera_entity;
		Ivory::OrthographicCameraController m_camera_controller;
		std::shared_ptr<Ivory::FrameBuffer> m_frame_buffer;

		glm::vec2 m_viewport_size{ 0.0f, 0.0f };
		glm::vec2 m_viewport_bounds[2];

		bool m_viewport_focused = false;
		bool m_viewport_hovered = false;

		bool m_willopen_scene = false;
		bool m_willsave_scene = false;

		bool m_willopen_project = false;
		bool m_willsave_project = false;

		Entity m_tethered_entity;
		enum class TetherType {
			Spring,
			Cable,
			Rod
		};
		TetherType m_tether_type;

		bool m_new_project = false;

		std::string m_current_scene_file = "";
		std::filesystem::path m_current_project_path = "";
		int m_gizmo = -1;
		bool m_using_gizmo = false;

		Entity m_entity_hovered;

		SceneHierarchy m_hierarchy;
		ProjectSetupWindow m_setup_window;
		WorldSettings m_world_settings;
		Timeline m_timeline;
		Grapher m_grapher;
		WaveViewport m_wave_viewport;
		Items m_items;
		Preferences m_preferences;

		ActionManager m_action_manager;
		

		std::string current_path;

		glm::vec2 m_first_mouse_coords{ 0.0f };
		SnapshotManager m_snapshot_manager{nullptr};

		bool m_show_viewport = true;
		bool m_show_timeline = true;
		bool m_show_hierarchy = true;
		bool m_show_world_settings = true;
		bool m_show_inspector = true;
		bool m_show_grapher = true;
		bool m_show_scene_graph = true;

		bool m_check_for_hold = false;
		bool m_mouse_holding = false;
		glm::vec2 m_selection[2];
	};

	class Editor : public Application {
	public:
		Editor() : Application("Zeus Sandbox") {
			//Ivory::Application::get_window().set_vsync(false);
			//push_layer(std::make_shared<Test2D>());
			push_layer(std::make_shared<EditorLayer>());

			get_window().set_image(icon_path);
		}
		~Editor() {}
	};
}

