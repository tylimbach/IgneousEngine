#include "log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace bve
{
	std::shared_ptr<spdlog::logger> Log::coreLogger_;
	std::shared_ptr<spdlog::logger> Log::clientLogger_;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		coreLogger_ = spdlog::stdout_color_mt("Engine");
		coreLogger_->set_level(spdlog::level::trace);

		clientLogger_ = spdlog::stdout_color_mt("App");
		clientLogger_->set_level(spdlog::level::trace);
	}
}
