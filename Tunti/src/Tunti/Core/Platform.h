#ifdef _WIN32
	#ifdef _WIN64
	#define PLATFORM_WINDOWS
	#else
	#error Tunti Engine currently does not support x86!
	#endif

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
	#error Tunti Engine currently does not support IOS simulator!
	#elif TARGET_OS_IPHONE == 1
	#define PLATFORM_IOS
	#error Tunti Engine currently does not support IOS!
	#elif TARGET_OS_MAC == 1
	#define PLATFORM_MACOS
	#error Tunti Engine currently does not support MacOS!
	#else
	#error Unknown Apple platform!
	#endif

#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#error Tunti Engine currently does not support Android!

#elif defined(__linux__)
#define PLATFORM_LINUX
#error Tunti Engine currently does not support Linux!

#else
#error Unknown platform!
#endif