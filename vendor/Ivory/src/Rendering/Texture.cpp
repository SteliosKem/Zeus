#include "pch.h"
#include "Texture.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Ivory {
	std::shared_ptr<Texture2D> Texture2D::create(uint32_t width, uint32_t height) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(width, height);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(path);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}
}