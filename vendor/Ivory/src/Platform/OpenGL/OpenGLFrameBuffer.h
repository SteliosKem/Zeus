#pragma once
#include "Rendering/FrameBuffer.h"

namespace Ivory {
	class OpenGLFrameBuffer : public FrameBuffer {
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		void bind() override;
		void unbind() override;

		void resize(uint32_t width, uint32_t height) override;
		int read_pixel(uint32_t attachment_index, int x, int y) override;

		void clear_attachment(uint32_t attachment_index, int value) override;

		uint32_t get_color_attachment_rendererID(uint32_t index = 0) const override {
			IV_CORE_ASSERT(index < m_color_attachments.size(), "Index out of bounds");
			return m_color_attachments[index]; 
		}

		const FrameBufferSpecification& get_spec() const override { return m_specification; }
		FrameBufferSpecification& get_spec() override { return m_specification; }
		void validate();
	private:
		uint32_t m_rendererID{ 0 };
		FrameBufferSpecification m_specification;

		std::vector<FrameBufferTextureSpecification> m_color_attachment_specs;
		FrameBufferTextureSpecification m_depth_attachment_spec;

		std::vector<uint32_t> m_color_attachments;
		uint32_t m_depth_attachment = 0;
	};
}