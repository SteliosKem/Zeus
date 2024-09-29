#pragma once

#include "Rendering/Texture.h"
#include <glad/glad.h>

namespace Ivory {
	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D();

		uint32_t get_width() const override { return m_width; }
		uint32_t get_height() const override { return m_height; }
		uint32_t get_rendererID() const override { return m_rendererID; }
		std::filesystem::path get_path() const override { return m_path; }

		void set_data(void* data, uint32_t size) override;

		void bind(uint32_t slot = 0) const override;

		bool operator==(const Texture& other) const override { return m_rendererID == ((OpenGLTexture2D&)other).m_rendererID; }
	private:
		std::string m_path;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_rendererID;
		GLenum m_internal_format, m_data_format;
	};
}