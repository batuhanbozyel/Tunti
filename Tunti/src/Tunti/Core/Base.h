#pragma once

#define BIT(x) (1 << x)

#ifdef DEBUG_ENABLED
#define LOG_ASSERT(x, ...) { if(!(x)) { ::Tunti::Log::Error("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define LOG_ASSERT(x, ...)
#endif

namespace Tunti
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using WeakRef = std::weak_ptr<T>;
}