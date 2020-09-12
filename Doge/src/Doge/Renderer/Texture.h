#pragma once

namespace Doge
{
	class ShaderStorageBuffer;
	class TextureManager;

	struct TextureMaps
	{
		uint64_t DiffuseMap;
		uint64_t SpecularMap;
	};

	enum class TextureType {
		Diffuse = offsetof(TextureMaps, DiffuseMap),
		Specular = offsetof(TextureMaps, SpecularMap)
	};

	class Texture
	{
	protected:
		virtual uint64_t GetTextureHandle() = 0;
	private:
		static std::unique_ptr<Texture> Create(const std::string& texturePath);
		static std::unique_ptr<Texture> CreateWhiteTexture();

		friend class TextureManager;
	};

	class TextureManager
	{
	public:
		static void Init();

		static uint32_t LoadTexture(const std::string& path);
		static uint32_t LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths);
	private:
		static std::vector<std::unique_ptr<Texture>> s_Textures;
		static std::shared_ptr<ShaderStorageBuffer> m_SSBO;

		static uint32_t m_Count;
		static std::unordered_map<std::string, uint32_t> s_TextureMap;
	};
}