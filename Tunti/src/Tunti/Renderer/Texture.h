#pragma once

namespace Tunti
{
	enum class TextureType : uint16_t
	{
		Albedo = 0,
		Normal,
		Metalness,
		Roughness,
		AmbientOcclusion,
		COUNT
	};

	static constexpr uint32_t OffsetofTextureType(TextureType type)
	{
		return static_cast<uint32_t>(type) * sizeof(uint64_t);
	}

	static constexpr uint32_t CalculateMipMapLevels(uint32_t width, uint32_t height)
	{
		uint32_t levels = 1;
		while ((width | height) >> levels) {
			++levels;
		}
		return levels;
	}

	static constexpr uint32_t MaxTextures = 1024;
	static constexpr uint32_t SizeofTextureMap = static_cast<uint32_t>(TextureType::COUNT) * sizeof(uint64_t);

	struct Texture2D
	{
		uint64_t Handle = 0;

		operator uint64_t () const { return Handle; }

		Texture2D() = default;

		Texture2D (uint64_t handle)
			: Handle(handle) {}
	};

	struct TextureMap
	{
		uint32_t Index = 0;
		std::array<Texture2D, static_cast<uint32_t>(TextureType::COUNT)> Textures{ 0 };

		TextureMap() = default;

		operator uint32_t() const { return Index; }
	};

	struct TextureData
	{
		unsigned char* buffer;
		int width, height, channel;
	};

	struct EnvironmentMapData
	{
		float* buffer;
		int width, height, channel;
	};

	struct EnvironmentMapTexture
	{
		uint32_t EnvironmentMapTextureID = 0;
		uint32_t IrradianceMapTextureID = 0;
		uint32_t BRDFto2DLUTTextureID = 0;
	};

	struct CubemapTexture
	{
		uint32_t TextureID = 0;

		operator uint32_t() const { return TextureID; }
	};

	class TextureLibrary final
	{
	public:
		static Ref<TextureMap> DefaultTextureMap();
		static Ref<TextureMap> LoadTextureMap(const std::array<std::string, static_cast<uint16_t>(TextureType::COUNT)>& textureFiles);
		static EnvironmentMapTexture LoadEnvironmentMap(const std::string& path);
		static CubemapTexture LoadCubemap(const std::string& folderPath,
			const std::string& rightFace,
			const std::string& leftFace,
			const std::string& topFace,
			const std::string& bottomFace,
			const std::string& frontFace,
			const std::string& backFace);
	};
}