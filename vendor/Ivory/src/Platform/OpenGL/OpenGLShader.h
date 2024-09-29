#pragma once
#include "Rendering/Shader.h"

using GLenum = unsigned int;

namespace Ivory {
	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);
		OpenGLShader(const std::string& file_path);
		~OpenGLShader() {}

		void bind() const override;
		void unbind() const override;

		const std::string& get_name() const override { return m_name; }

		void set_mat4(const std::string& name, const glm::mat4& mat) override;
		void set_vec4(const std::string& name, const glm::vec4& vec) override;
		void set_vec3(const std::string& name, const glm::vec3& vec) override;
		void set_int(const std::string& name, int num) override;
		void set_int_array(const std::string& name, int* data, uint32_t count) override;
		void set_float(const std::string& name, float num) override;

		void upload_uniform_mat4(const std::string& name, const glm::mat4& matrix);
		void upload_uniform_mat3(const std::string& name, const glm::mat3& matrix);

		void upload_uniform_int(const std::string& name, int num);
		void upload_uniform_int_array(const std::string& name, int* data, uint32_t count);
		void upload_uniform_vec4(const std::string& name, const glm::vec4& vector);
		void upload_uniform_vec3(const std::string& name, const glm::vec3& vector);
		void upload_uniform_vec2(const std::string& name, const glm::vec2& vector);
		void upload_uniform_float(const std::string& name, float num);
	private:
		void compile(const std::unordered_map<GLenum, std::string>& shader_srcs);
		std::unordered_map<GLenum, std::string> preprocess(const std::string& source);

		uint32_t m_rendererID;
		std::string m_name;
	};
}