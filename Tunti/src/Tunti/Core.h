#pragma once

#ifdef T_PLATFORM_WINDOWS
	#if T_DYNAMIC_LINK
		#ifdef T_BUILD_DLL
			#define TUNTI_API __declspec(dllexport)
		#else
			#define TUNTI_API __declspec(dllimport)
		#endif
	#else
		#define TUNTI_API
	#endif
#else
	#error Tunti only supports Windows!
#endif

#ifdef T_ENABLE_ASSERTS
	#define T_ASSERT(x, ...) { if(!(x)) { T_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define T_CORE_ASSERT(x, ...) { if(!(x)) { T_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define T_ASSERT(x, ...)
	#define T_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define T_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)