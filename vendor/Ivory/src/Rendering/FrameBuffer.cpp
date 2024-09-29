#include "pch.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Ivory {
	std::shared_ptr<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification& spec) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLFrameBuffer>(spec);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}
}