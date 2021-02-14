#include "pch.h"
#include "Tunti/Core/Platform.h"
#include "Tunti/Utility/PlatformUtils.h"

#ifdef PLATFORM_WINDOWS

namespace Tunti
{
	namespace FileManager
	{
		std::string GetFileExtension(const std::string& path)
		{
			return PathFindExtensionA(path.c_str());
		}
	}
#else
#error Tunti Engine currently supports Windows only!
#endif
}