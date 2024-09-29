#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <filesystem>

namespace Ivory {
	

	class Texture {
	public:
		virtual ~Texture() = default;

		virtual uint32_t get_width() const = 0;
		virtual uint32_t get_height() const = 0;
		virtual uint32_t get_rendererID() const = 0;
		virtual std::filesystem::path get_path() const = 0;

		virtual void set_data(void* data, uint32_t size) = 0;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture {
	public:
		static std::shared_ptr<Texture2D> create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture2D> create(const std::string& path);
	};
}