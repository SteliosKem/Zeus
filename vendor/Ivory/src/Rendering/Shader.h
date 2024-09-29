#pragma once
#include <string>
#include <glm/glm.hpp>
#include <memory>

namespace Ivory {
	class Shader {
	public:
		virtual ~Shader() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void set_mat4(const std::string& name, const glm::mat4& mat) = 0;
		virtual void set_vec4(const std::string& name, const glm::vec4& vec) = 0;
		virtual void set_vec3(const std::string& name, const glm::vec3& vec) = 0;
		virtual void set_int(const std::string& name, int num) = 0;
		virtual void set_int_array(const std::string& name, int* data, uint32_t count) = 0;
		virtual void set_float(const std::string& name, float num) = 0;

		virtual const std::string& get_name() const = 0;

		static std::shared_ptr<Shader> create(const std::string& file_path);
		static std::shared_ptr<Shader> create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);
	};

	class ShaderLibrary {
	public:
		void add(const std::shared_ptr<Shader>& shader);
		std::shared_ptr<Shader> load(const std::string& file_path);

		std::shared_ptr<Shader> get(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	};
}