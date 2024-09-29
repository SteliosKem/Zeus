#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace Ivory {
	std::shared_ptr<Shader> Shader::create(const std::string& file_path) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(file_path);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src) {
		switch (Renderer::get_api()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertex_src, fragment_src);
		case RendererAPI::API::None:
		default:
			IV_CORE_ASSERT("Rendering API not supported");
		}
		return nullptr;
	}

	void ShaderLibrary::add(const std::shared_ptr<Shader>& shader) {
		const std::string& name = shader->get_name();
		IV_CORE_ASSERT(m_shaders.find(name) == m_shaders.end(), "Shader already exists");
		
		m_shaders[name] = shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::load(const std::string& file_path) {
		std::shared_ptr<Shader> shader = Shader::create(file_path);
		add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::get(const std::string& name) {
		IV_CORE_ASSERT(m_shaders.find(name) != m_shaders.end(), "Shader not found");
		return m_shaders[name];
	}
}