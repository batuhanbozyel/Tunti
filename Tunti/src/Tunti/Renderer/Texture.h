#pragma once

namespace Tunti
{
	static constexpr uint32_t CalculateMipMapLevels(uint32_t width, uint32_t height)
	{
		uint32_t levels = 1;
		while ((width | height) >> levels) {
			++levels;
		}
		return levels;
	}

	struct Texture2D
	{
		uint64_t Handle = 0;

		operator uint64_t () const { return Handle; }

		explicit Texture2D() = default;
		explicit Texture2D (uint64_t handle)
			: Handle(handle) {}
	};

	enum class PBRTextureMap : uint16_t
	{
		Albedo,
		Normal,
		Metalness,
		Roughness,
		AmbientOcclusion,
		COUNT
	};

	struct PBRTextureMaps
	{
		Texture2D Albedo;
		Texture2D Normal;
		Texture2D Metalness;
		Texture2D Roughness;
		Texture2D AmbientOcclusion;
	};

	struct TextureData
	{
		unsigned char* buffer;
		int width, height, channel;
	};

	struct EnvironmentMapTexture
	{
		uint32_t EnvironmentMapTextureID = 0;
		uint32_t IrradianceMapTextureID = 0;
		uint32_t BRDF_LUTTextureID = 0;
	};

	struct CubemapTexture
	{
		uint32_t TextureID = 0;

		operator uint32_t() const { return TextureID; }
	};

	class TextureLibrary final
	{
	public:
		static PBRTextureMaps DefaultPBRTextureMaps();
		static EnvironmentMapTexture DefaultEnvironmentMap();

		static PBRTextureMaps LoadTextureMaps(const std::array<std::string, static_cast<uint16_t>(PBRTextureMap::COUNT)>& textureFiles);
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