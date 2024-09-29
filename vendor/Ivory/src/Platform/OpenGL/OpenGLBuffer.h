#pragma once
#include "Rendering/Buffer.h"
#include <cstdint>

namespace Ivory {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);
		virtual ~OpenGLVertexBuffer();

		void bind() const override;
		void unbind() const override;

		void set_data(const void* data, uint32_t size) override;
		void set_layout(const BufferLayout& layout) { m_layout = layout; }
		const BufferLayout& get_layout() { return m_layout; }
	private:
		uint32_t m_rendererID;
		BufferLayout m_layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t* indeces, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		uint32_t get_count() { return m_count; }

		void bind() const override;
		void unbind() const override;
	private:
		uint32_t m_rendererID;
		uint32_t m_count;
	};

	class OpenGLVertexArray : public VertexArray {
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		void bind() const override;
		void unbind() const override;

		void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) override;
		void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) override;

		const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const { return m_vertex_buffers; }
		const std::shared_ptr<IndexBuffer>& get_index_buffer() const { return m_index_buffer; }
	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_vertex_buffers;
		std::shared_ptr<IndexBuffer> m_index_buffer;
		uint32_t m_rendererID;
	};
}