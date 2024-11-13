#pragma once
#include <cstdint>
#include <memory>

namespace Ivory {
	enum class FrameBufferTextureFormat {
		None = 0,

		//Color
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		Depth = DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification {
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format) : texture_format(format) {}

		FrameBufferTextureFormat texture_format = FrameBufferTextureFormat::None;
	};

	struct FrameBufferAttachmentSpecification {
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(const std::initializer_list<FrameBufferTextureSpecification>& _attachments)
		: attachments(_attachments) {}

		std::vector<FrameBufferTextureSpecification> attachments;
	};

	struct FrameBufferSpecification {
		uint32_t width, height;
		FrameBufferAttachmentSpecification attachments;
		uint32_t samples = 1;

		bool swap_chain_target = false;
	};

	class FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;
		virtual void bind() = 0;
		virtual void bind_tex() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;
		virtual int read_pixel(uint32_t attachment_index, int x, int y) = 0;

		virtual void clear_attachment(uint32_t attachment_index, int value) = 0;

		virtual uint32_t get_color_attachment_rendererID(uint32_t index = 0) const = 0;

		virtual const FrameBufferSpecification& get_spec() const = 0;
		virtual FrameBufferSpecification& get_spec() = 0;
		static std::shared_ptr<FrameBuffer> create(const FrameBufferSpecification& spec);
	};
}