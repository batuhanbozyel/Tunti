#pragma once

#include "Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Tunti
{
	class TUNTI_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define T_CORE_TRACE(...)   ::Tunti::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define T_CORE_INFO(...)    ::Tunti::Log::GetCoreLogger()->info(__VA_ARGS__)
#define T_CORE_WARN(...)    ::Tunti::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define T_CORE_ERROR(...)   ::Tunti::Log::GetCoreLogger()->error(__VA_ARGS__)
#define T_CORE_FATAL(...)   ::Tunti::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define T_TRACE(...)   ::Tunti::Log::GetClientLogger()->trace(__VA_ARGS__)
#define T_INFO(...)    ::Tunti::Log::GetClientLogger()->info(__VA_ARGS__)
#define T_WARN(...)    ::Tunti::Log::GetClientLogger()->warn(__VA_ARGS__)
#define T_ERROR(...)   ::Tunti::Log::GetClientLogger()->error(__VA_ARGS__)
#define T_FATAL(...)   ::Tunti::Log::GetClientLogger()->critical(__VA_ARGS__)

