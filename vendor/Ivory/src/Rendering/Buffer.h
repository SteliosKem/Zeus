#pragma once
#include <vector>

namespace Ivory {
	enum class ShaderDataType {
		None = 0,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Mat3,
		Mat4,
		Int,
		IVector2,
		IVector3,
		IVector4,
		Bool
	};

	static uint32_t shader_type_size(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float: return 4;
		case ShaderDataType::Vector2: return 4 * 2;
		case ShaderDataType::Vector3: return 4 * 3;
		case ShaderDataType::Vector4: return 4 * 4;
		case ShaderDataType::Mat3: return 4 * 3 * 3;
		case ShaderDataType::Mat4: return 4 * 4 * 4;
		case ShaderDataType::Int: return 4;
		case ShaderDataType::IVector2: return 4 * 2;
		case ShaderDataType::IVector3: return 4 * 3;
		case ShaderDataType::IVector4: return 4 * 4;
		case ShaderDataType::Bool: return 1;
		default:
			IV_CORE_ASSERT(false, "Uknown shader data type");
			return 0;
		}
	}

	class BufferElement {
	public:
		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), shader_type(type), size(shader_type_size(type)), normalized(normalized) {}

		uint32_t get_count() const {
			switch (shader_type) {
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Vector2: return 2;
			case ShaderDataType::Vector3: return 3;
			case ShaderDataType::Vector4: return 4;
			case ShaderDataType::Int: return 1;
			case ShaderDataType::IVector2: return 2;
			case ShaderDataType::IVector3: return 3;
			case ShaderDataType::IVector4: return 4;
			case ShaderDataType::Bool: return 1;
			}
		}

		std::string name;
		ShaderDataType shader_type;
		uint32_t size{ 0 };
		uint32_t offset{ 0 };
		bool normalized{ false };
	};

	class BufferLayout {
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements) {
			calc_offset_stride();
		}
		inline const std::vector<BufferElement>& get_elements() const { return m_elements; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

		inline uint32_t get_stride() const { return m_stride; }
	private:
		void calc_offset_stride() {
			uint32_t offset = 0;
			m_stride = 0;
			for (BufferElement& element : m_elements) {
				element.offset = offset;
				offset += element.size;
				m_stride += element.size;
			}
		}
		std::vector<BufferElement> m_elements;
		uint32_t m_stride;
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void set_layout(const BufferLayout& layout) = 0;
		virtual void set_data(const void* data, uint32_t size) = 0;
		virtual const BufferLayout& get_layout() = 0;

		static std::shared_ptr<VertexBuffer> create_buffer(float* vertices, uint32_t size);
		static std::shared_ptr<VertexBuffer> create_buffer(uint32_t size);
	};

	class IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t get_count() = 0;

		static std::shared_ptr<IndexBuffer> create_buffer(uint32_t* indeces, uint32_t count);
	};

	class VertexArray {
	public:
		virtual ~VertexArray() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) = 0;
		virtual void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& get_index_buffer() const = 0;

		static std::shared_ptr<VertexArray> create_array();
	};
}