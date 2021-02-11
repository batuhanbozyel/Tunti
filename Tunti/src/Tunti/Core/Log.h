#pragma once
#include "Base.h"
#include <spdlog/spdlog.h>

namespace Tunti
{
	class Log
	{
	public:
		static void Init();

		template<typename ... Args>
		static void Trace(Args&& ... args) { s_Logger->trace(std::forward<Args>(args)...); }

		template<typename ... Args>
		static void Info(Args&& ... args) { s_Logger->info(std::forward<Args>(args)...); }

		template<typename ... Args>
		static void Warn(Args&& ... args) { s_Logger->warn(std::forward<Args>(args)...); }

		template<typename ... Args>
		static void Error(Args&& ... args) { s_Logger->error(std::forward<Args>(args)...); }

		template<typename ... Args>
		static void Critical(Args&& ... args) { s_Logger->critical(std::forward<Args>(args)...); }
	private:
		static Ref<spdlog::logger> s_Logger;
	};
}