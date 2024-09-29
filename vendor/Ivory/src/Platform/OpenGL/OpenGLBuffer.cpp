#include "pch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Ivory {
	static GLenum shader_to_opengl_type(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float: return GL_FLOAT;
		case ShaderDataType::Vector2: return GL_FLOAT;
		case ShaderDataType::Vector3: return GL_FLOAT;
		case ShaderDataType::Vector4: return GL_FLOAT;
		case ShaderDataType::Int: return GL_INT;
		case ShaderDataType::IVector4: return GL_INT;
		case ShaderDataType::IVector3: return GL_INT;
		case ShaderDataType::IVector2: return GL_INT;

		}
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
		glCreateBuffers(1, &m_rendererID);
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
		glCreateBuffers(1, &m_rendererID);
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLVertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLVertexBuffer::unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::set_data(const void* data, uint32_t size) {
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// INDEX BUFFER
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indeces, uint32_t count) : m_count(count) {
		glCreateBuffers(1, &m_rendererID);
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indeces, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLIndexBuffer::bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLIndexBuffer::unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// VERTEX ARRAY

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) {
		IV_CORE_ASSERT(vertex_buffer->get_layout().get_elements().size(), "Vertex buffer has no layout");

		glBindVertexArray(m_rendererID);
		vertex_buffer->bind();

		uint32_t i = 0;
		for (const BufferElement& element : vertex_buffer->get_layout()) {
			switch (element.shader_type) {
			case ShaderDataType::Float:
			case ShaderDataType::Vector2:
			case ShaderDataType::Vector3:
			case ShaderDataType::Vector4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, element.get_count(), shader_to_opengl_type(element.shader_type),
					element.normalized ? GL_TRUE : GL_FALSE, vertex_buffer->get_layout().get_stride(), (const void*)element.offset);
				i++;
				break;
			case ShaderDataType::Int:
			case ShaderDataType::IVector2:
			case ShaderDataType::IVector3:
			case ShaderDataType::IVector4:
				glEnableVertexAttribArray(i);
				glVertexAttribIPointer(i, element.get_count(), shader_to_opengl_type(element.shader_type),
					vertex_buffer->get_layout().get_stride(), (const void*)element.offset);
				i++;
				break;
			}
		}
		m_vertex_buffers.push_back(vertex_buffer);
	}

	void OpenGLVertexArray::set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) {
		glBindVertexArray(m_rendererID);
		index_buffer->bind();

		m_index_buffer = index_buffer;
	}
}
