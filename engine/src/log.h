#pragma once

#include "spdlog/spdlog.h"
#include "defines.h"

#include <memory.h>

namespace bve 
{
	class KAPI Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return coreLogger_; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return clientLogger_; }
	private:
		static std::shared_ptr<spdlog::logger> coreLogger_;
		static std::shared_ptr<spdlog::logger> clientLogger_;
	};
}

#define LOG_FATAL(...) ::bve::Log::getCoreLogger()->error(__VA_ARGS__)
#define LOG_ERROR(...) ::bve::Log::getCoreLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::bve::Log::getCoreLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::bve::Log::getCoreLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::bve::Log::getCoreLogger()->trace(__VA_ARGS__)

#define LOG_APP_FATAL(...) ::bve::Log::getClientLogger()->error(__VA_ARGS__)
#define LOG_APP_ERROR(...) ::bve::Log::getClientLogger()->error(__VA_ARGS__)
#define LOG_APP_WARN(...) ::bve::Log::getClientLogger()->warn(__VA_ARGS__)
#define LOG_APP_INFO(...) ::bve::Log::getClientLogger()->info(__VA_ARGS__)
#define LOG_APP_TRACE(...) ::bve::Log::getClientLogger()->trace(__VA_ARGS__)