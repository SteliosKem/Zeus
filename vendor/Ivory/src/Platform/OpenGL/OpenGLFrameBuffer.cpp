#include "pch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Ivory {
	static const uint32_t s_max_frame_buffer_size = 8192;

	static GLenum texture_target(bool multisampled) {
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void create_textures(bool multisampled, uint32_t* out_id, uint32_t count) {
		glCreateTextures(texture_target(multisampled), count, out_id);
	}

	static void bind_texture(bool multisampled, uint32_t id) {
		glBindTexture(texture_target(multisampled), id);
	}

	static GLenum to_gl_format(FrameBufferTextureFormat format) {
		switch (format) {
		case FrameBufferTextureFormat::RGBA8:
			return GL_RGBA8;
		case FrameBufferTextureFormat::RED_INTEGER:
			return GL_RED_INTEGER;
		}

		IV_CORE_ASSERT(false, "Unkown format");
		return 0;
	}

	static void attach_depth_texture(uint32_t id, int samples, GLenum format, GLenum attachment_type, uint32_t width, uint32_t height) {
		bool multisampled = samples > 1;
		if (multisampled)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		else {
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, texture_target(multisampled), id, 0);
	}

	static void attach_color_texture(uint32_t id, int samples, GLenum internal_format, GLenum format, uint32_t width, uint32_t height, int index) {
		bool multisampled = samples > 1;
		if (multisampled)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_FALSE);
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture_target(multisampled), id, 0);
	}

	static bool is_depth_format(FrameBufferTextureFormat format) {
		switch (format) {
		case FrameBufferTextureFormat::DEPTH24STENCIL8:
			return true;
		}
		return false;
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec) : m_specification(spec) {
		for (auto format : m_specification.attachments.attachments) {
			if (!is_depth_format(format.texture_format))
				m_color_attachment_specs.emplace_back(format);
			else
				m_depth_attachment_spec = format;
		}

		validate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer() {
		glDeleteFramebuffers(1, &m_rendererID);
		glDeleteTextures(m_color_attachments.size(), m_color_attachments.data());
		glDeleteTextures(1, &m_depth_attachment);
	}

	void OpenGLFrameBuffer::validate() {
		if (m_rendererID) {
			glDeleteFramebuffers(1, &m_rendererID);
			glDeleteTextures(m_color_attachments.size(), m_color_attachments.data());
			glDeleteTextures(1, &m_depth_attachment);

			m_color_attachments.clear();
			m_depth_attachment = 0;
		}
		glCreateFramebuffers(1, &m_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

		bool multisample = m_specification.samples > 1;

		if (m_color_attachment_specs.size()) {
			m_color_attachments.resize(m_color_attachment_specs.size());
			create_textures(multisample, m_color_attachments.data(), m_color_attachments.size());

			for (size_t i = 0; i < m_color_attachment_specs.size(); i++) {
				bind_texture(multisample, m_color_attachments[i]);
				switch (m_color_attachment_specs[i].texture_format) {
				case FrameBufferTextureFormat::RGBA8:
					attach_color_texture(m_color_attachments[i], m_specification.samples, GL_RGBA8, GL_RGBA, m_specification.width, m_specification.height, i);
					break;
				case FrameBufferTextureFormat::RED_INTEGER:
					attach_color_texture(m_color_attachments[i], m_specification.samples, GL_R32I, GL_RED_INTEGER, m_specification.width, m_specification.height, i);
					break;
				}
			}
		}

		if (m_depth_attachment_spec.texture_format != FrameBufferTextureFormat::None) {
			create_textures(multisample, &m_depth_attachment, 1);
			bind_texture(multisample, m_depth_attachment);
			switch (m_depth_attachment_spec.texture_format) {
			case FrameBufferTextureFormat::DEPTH24STENCIL8:
				attach_depth_texture(m_depth_attachment, m_specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_specification.width, m_specification.height);
				break;
			}
		}

		if (m_color_attachments.size() > 1) {
			IV_CORE_ASSERT(m_color_attachments.size() <= 4, "More than 4 color attachments are not supported");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_color_attachments.size(), buffers);
		}
		else if (m_color_attachments.empty()) {
			glDrawBuffer(GL_NONE);
		}

		/*glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment);
		glBindTexture(GL_TEXTURE_2D, m_color_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specification.width, m_specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_attachment);
		glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_specification.width, m_specification.height);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specification.width, m_specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_attachment, 0);*/
		IV_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frame buffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void OpenGLFrameBuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
		glViewport(0, 0, m_specification.width, m_specification.height);

	}

	void OpenGLFrameBuffer::bind_tex() {
		glBindTexture(GL_TEXTURE_2D, get_color_attachment_rendererID());
	}

	void OpenGLFrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height) {
		if (width == 0 || height == 0 || width > s_max_frame_buffer_size || height > s_max_frame_buffer_size) {
			IV_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_specification.width = width;
		m_specification.height = height;
		validate();
	}

	int OpenGLFrameBuffer::read_pixel(uint32_t attachment_index, int x, int y) {
		IV_CORE_ASSERT(attachment_index < m_color_attachments.size(), "Index out of bounds");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
		int pixel_data;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
		return pixel_data;
	}

	void OpenGLFrameBuffer::clear_attachment(uint32_t attachment_index, int value) {
		IV_CORE_ASSERT(attachment_index < m_color_attachments.size(), "Index out of bounds");

		auto& spec = m_color_attachment_specs[attachment_index];
		glClearTexImage(m_color_attachments[attachment_index], 0, to_gl_format(spec.texture_format), GL_INT, &value);
	}
}