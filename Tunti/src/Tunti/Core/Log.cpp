#include "pch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Tunti
{
	Ref<spdlog::logger> Log::s_Logger = nullptr;

	void Log::Init()
	{
		if (s_Logger == nullptr)
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("Doge.log", true));

			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			s_Logger = CreateRef<spdlog::logger>("DOGE", std::begin(logSinks), std::end(logSinks));
			spdlog::register_logger(s_Logger);
			s_Logger->set_level(spdlog::level::trace);
			s_Logger->flush_on(spdlog::level::trace);
		}
	}
}