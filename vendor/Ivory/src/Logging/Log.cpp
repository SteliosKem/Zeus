#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Ivory {
	std::shared_ptr<spdlog::logger> Log::m_core_logger;
	std::shared_ptr<spdlog::logger> Log::m_client_logger;

	void Log::init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_core_logger = spdlog::stdout_color_mt("Ivory");
		m_core_logger->set_level(spdlog::level::trace);
		m_client_logger = spdlog::stdout_color_mt("Client");
		m_client_logger->set_level(spdlog::level::trace);
	}
}