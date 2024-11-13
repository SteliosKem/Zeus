#include "pch.h"
#include "Rendering/Renderer.h"
#include "OpenGLShader.h"

#include "Core/Util.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Ivory {
	static GLenum get_shader_type(const std::string& type) {
		if (type == "vertex") return GL_VERTEX_SHADER;
		else if (type == "fragment") return GL_FRAGMENT_SHADER;
		IV_CORE_ASSERT(false, "Unknown shader type");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& file_path) {
		std::filesystem::path path = file_path;
		m_name = path.stem().string();
		std::string file = read_file(file_path);
		compile(preprocess(file));
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_src) {
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex_src;
		sources[GL_FRAGMENT_SHADER] = fragment_src;
		compile(sources);
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shader_srcs) {
		GLuint program = glCreateProgram();
		std::vector<GLenum> glShader_IDs;

		for (auto& kv : shader_srcs) {
			GLenum type = kv.first;
			const std::string& src = kv.second;
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const char* source = src.c_str();
			glShaderSource(shader, 1, &source, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog as you see fit.

				IV_CORE_ERROR("Shader compilation failed");
				IV_CORE_ERROR((const char*)infoLog.data());
				IV_CORE_ASSERT(false, "Shader compilation failed");
				break;
			}
			glAttachShader(program, shader);
			glShader_IDs.push_back(shader);
		}
		m_rendererID = program;


		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : glShader_IDs)
				glDeleteShader(id);

			// Use the infoLog as you see fit.

			// In this simple program, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShader_IDs)
			glDetachShader(program, id);
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const {
		glDeleteProgram(m_rendererID);
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) {
		std::unordered_map<GLenum, std::string> shader_srcs;

		const char* type_token = "#type";
		size_t token_length = strlen(type_token);
		size_t pos = source.find(type_token, 0);
		while (pos != std::string::npos) {
			size_t end_of_line = source.find_first_of("\r\n", pos);
			IV_CORE_ASSERT(end_of_line != std::string::npos, "Syntax error");
			end_of_line == std::string::npos ? IV_CORE_ERROR("Syntax error") : 0;
			size_t begin = pos + token_length + 1;
			std::string type = source.substr(begin, end_of_line - begin);
			IV_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid type specifier '" + type + "'");
			!(type == "vertex" || type == "fragment" || type == "pixel") ? IV_CORE_ERROR("Invalid type specifier '" + type + "'") : 0;

			size_t next_line_pos = source.find_first_not_of("\r\n", end_of_line);
			pos = source.find(type_token, next_line_pos);
			shader_srcs[get_shader_type(type)] = source.substr(next_line_pos, pos - (next_line_pos == std::string::npos ? source.size() - 1 : next_line_pos));
		}

		return shader_srcs;
	}

	void OpenGLShader::upload_uniform_mat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::upload_uniform_mat3(const std::string& name, const glm::mat3& matrix) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::upload_uniform_vec3(const std::string& name, const glm::vec3& vector) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, vector.x, vector.y, vector.z);
	}
	void OpenGLShader::upload_uniform_vec4(const std::string& name, const glm::vec4& vector) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
	}
	void OpenGLShader::upload_uniform_vec2(const std::string& name, const glm::vec2& vector) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, vector.x, vector.y);
	}
	void OpenGLShader::upload_uniform_float(const std::string& name, float num) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, num);
	}
	void OpenGLShader::upload_uniform_int(const std::string& name, int num) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, num);
	}
	void OpenGLShader::upload_uniform_int_array(const std::string& name, int* data, uint32_t count) {
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(location, count, data);
	}

	void OpenGLShader::set_mat4(const std::string& name, const glm::mat4& mat) {
		upload_uniform_mat4(name, mat);
	}

	void OpenGLShader::set_vec4(const std::string& name, const glm::vec4& vec) {
		upload_uniform_vec4(name, vec);
	}

	void OpenGLShader::set_vec3(const std::string& name, const glm::vec3& vec) {
		upload_uniform_vec3(name, vec);
	}

	void OpenGLShader::set_vec2(const std::string& name, const glm::vec2& vec) {
		upload_uniform_vec2(name, vec);
	}

	void OpenGLShader::set_int(const std::string& name, int num) {
		upload_uniform_int(name, num);
	}

	void OpenGLShader::set_float(const std::string& name, float num) {
		upload_uniform_float(name, num);
	}

	void OpenGLShader::set_int_array(const std::string& name, int* data, uint32_t count) {
		upload_uniform_int_array(name, data, count);
	}
}