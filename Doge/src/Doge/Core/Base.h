#pragma once

#ifdef DEBUG_ENABLED
#define LOG_ASSERT(x, ...) { if(!(x)) { ::Doge::Log::Error("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define LOG_ASSERT(x, ...)
#endif

namespace Doge
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

	enum TextureBinding : uint16_t
	{
		QuadFramebufferColorAttachment = 0,
		SkyboxTextureUnit = 1
	};

	enum BufferBinding : uint16_t
	{
		ViewProjectionUniformBuffer = 1,
		LightingUniformBuffer = 2
	};
}