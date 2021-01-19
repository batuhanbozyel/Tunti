#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	enum class TextureType : uint16_t
	{
		Albedo,
		Normal,
		Metallic,
		Roughness,
		AmbientOcclusion,
		COUNT
	};

	static uint32_t OffsetofTextureType(TextureType type)
	{
		return static_cast<uint32_t>(type) * sizeof(uint64_t);
	}

	static constexpr uint32_t MaxTextures = 1024;
	static constexpr uint32_t SizeofTextureMap = static_cast<uint32_t>(TextureType::COUNT) * sizeof(uint64_t);

	struct Texture2D
	{
		uint32_t Index = 0;

		operator uint32_t() const { return Index; }
	};

	struct TextureData
	{
		unsigned char* buffer;
		int width, height, channel;
	};

	struct CubemapTexture
	{
		uint32_t TextureID = 0;

		operator uint32_t() const { return TextureID; }
	};

	class TextureLibrary
	{
	public:
		static constexpr Texture2D DefaultTexture = Texture2D();
	public:
		static Texture2D LoadTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles);
		static CubemapTexture LoadCubemap(const std::string& folderPath,
			const std::string& rightFace,
			const std::string& leftFace,
			const std::string& topFace,
			const std::string& bottomFace,
			const std::string& frontFace,
			const std::string& backFace);
	};
}