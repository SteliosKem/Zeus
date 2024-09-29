#pragma once
#include "Core/Core.h"
#include "spdlog/spdlog.h"

namespace Ivory {
	// Wrapper for spdlog
	class IVORY_API Log
	{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& get_core_logger() {
			return m_core_logger;
		}
		inline static std::shared_ptr<spdlog::logger>& get_client_logger() {
			return m_client_logger;
		}
	private:
		static std::shared_ptr<spdlog::logger> m_core_logger;
		static std::shared_ptr<spdlog::logger> m_client_logger;
	};



#define IV_CORE_ERROR(...) ::Ivory::Log::get_core_logger()->error(__VA_ARGS__)
#define IV_CORE_WARN(...) ::Ivory::Log::get_core_logger()->warn(__VA_ARGS__)
#define IV_CORE_INFO(...) ::Ivory::Log::get_core_logger()->info(__VA_ARGS__)
#define IV_CORE_TRACE(...) ::Ivory::Log::get_core_logger()->trace(__VA_ARGS__)
#define IV_CORE_FATAL(...) ::Ivory::Log::get_core_logger()->fatal(__VA_ARGS__)

#define IV_ERROR(...) ::Ivory::Log::get_client_logger()->error(__VA_ARGS__)
#define IV_WARN(...) ::Ivory::Log::get_client_logger()->warn(__VA_ARGS__)
#define IV_INFO(...) ::Ivory::Log::get_client_logger()->info(__VA_ARGS__)
#define IV_TRACE(...) ::Ivory::Log::get_client_logger()->trace(__VA_ARGS__)
#define IV_FATAL(...) ::Ivory::Log::get_client_logger()->fatal(__VA_ARGS__)
}

