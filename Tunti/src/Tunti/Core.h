#pragma once

#ifdef T_PLATFORM_WINDOWS
	#ifdef T_BUILD_DLL
		#define TUNTI_API __declspec(dllexport)
	#else
		#define TUNTI_API __declspec(dllimport)
	#endif
#else
	#error Tunti only supports Windows!
#endif