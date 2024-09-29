#include "pch.h"
#include "SubTexture.h"

namespace Ivory {
	SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max) : m_texture(texture) {
		m_texture_coords[0] = { min.x, min.y };
		m_texture_coords[1] = { max.x, min.y };
		m_texture_coords[2] = { max.x, max.y };
		m_texture_coords[3] = { min.x, max.y };
	}

	std::shared_ptr<SubTexture2D> SubTexture2D::create_from_coords(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cell_size, const glm::vec2& sprite_size, float space_between) {
		glm::vec2 min = { ((coords.x * cell_size.x) + coords.x * space_between) / texture->get_width(), ((coords.y * cell_size.y) + coords.y * space_between) / texture->get_height() };
		glm::vec2 max = { (((coords.x + sprite_size.x) * cell_size.x) + coords.x * space_between) / texture->get_width(), (((coords.y + sprite_size.y) * cell_size.y) + coords.y * space_between) / texture->get_height() };
		return std::make_shared<SubTexture2D>(texture, min, max);
	}
}