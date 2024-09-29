#pragma once
#include "Texture.h"
#include <glm/glm.hpp>

namespace Ivory {
	class SubTexture2D {
	public:
		SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const std::shared_ptr<Texture2D>& get_texture() const { return m_texture; }
		std::array<glm::vec2, 4> get_texture_coords() const { return m_texture_coords; }

		static std::shared_ptr<SubTexture2D> create_from_coords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cell_size, const glm::vec2& sprite_size = {1.0f, 1.0f}, float space_between = 0.0f);
	private:
		std::shared_ptr<Texture2D> m_texture;
		std::array<glm::vec2, 4> m_texture_coords;
	};
}