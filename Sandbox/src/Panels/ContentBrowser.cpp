#include "ContentBrowser.h"
#include <string>
#include <fstream>
#include "imgui.h"

namespace Ivory {
	

	void ContentBrowser::on_imgui_render() {
		static const float padding = 64.0f;
		static const float thumbnail_size = 96.0f;
		float cell_size = thumbnail_size + padding;

		float panel_width = ImGui::GetContentRegionAvail().x;
		int column_count = (int)(panel_width / cell_size);
		if (column_count < 1)
			column_count = 1;

		ImGui::Begin("Content Browser");

		if (m_current_dir != m_assets_dir) {
			if (ImGui::Button("Back")) {
				m_current_dir = m_current_dir.parent_path();
			}
		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Folder")) {
				std::filesystem::create_directory(m_current_dir / "New Folder");
			}
			if (ImGui::MenuItem("Create File")) {
				std::ofstream s(m_current_dir / "New File.txt");
				s.close();
			}
			ImGui::EndPopup();
		}

		//if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) {}

		ImGui::Columns(column_count, 0, false);

		int i = 0;
		for (auto& p : std::filesystem::directory_iterator(m_current_dir)) {
			ImGui::PushID(i++);
			const auto& path = p.path();
			auto relative_path = std::filesystem::relative(p.path(), m_assets_dir);
			std::string name = relative_path.filename().string();

			std::shared_ptr<Texture2D> icon;
			if (p.is_directory())
				icon = m_folder_texture;
			else
				icon = m_file_texture;
			// WILL ADD DIFFERENT TEXTURES FOR EACH FILE
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->get_rendererID(), { thumbnail_size, thumbnail_size }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource()) {
				const wchar_t* item_path = relative_path.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", item_path, wcslen(item_path) * sizeof(wchar_t) + 2, ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (p.is_directory()) {
					m_current_dir /= p.path().filename();
				}
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Delete"));
				if (ImGui::MenuItem("Duplicate"));
				if (ImGui::MenuItem("Rename"));
				ImGui::EndPopup();
			}
			ImGui::TextWrapped(name.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::End();
	}
}