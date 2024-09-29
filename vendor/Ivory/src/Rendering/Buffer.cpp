#include "pch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Ivory {
	std::shared_ptr<VertexBuffer> VertexBuffer::create_buffer(float* vertices, uint32_t size) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::create_buffer(uint32_t size) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::create_buffer(uint32_t* indeces, uint32_t size) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(indeces, size);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	std::shared_ptr<VertexArray> VertexArray::create_array() {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}
}