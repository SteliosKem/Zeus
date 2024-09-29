#pragma once
#include <filesystem>
#include <Rendering/Texture.h>

static std::string icon_folder_path = "Assets/Textures/Icons/";

namespace Ivory {
	class ContentBrowser {
	public:
		ContentBrowser() {
			m_folder_texture = Texture2D::create(icon_folder_path + "folder.png");
			m_file_texture = Texture2D::create(icon_folder_path + "file.png");
		}
		void on_imgui_render();
		void set_assets_dir(const std::filesystem::path& path) { m_assets_dir = path; m_current_dir = path; }
	private:
		std::filesystem::path m_assets_dir = "Assets";
		std::filesystem::path m_current_dir = m_assets_dir;
		
		std::shared_ptr<Texture2D> m_folder_texture;
		std::shared_ptr<Texture2D> m_file_texture;
	};
}