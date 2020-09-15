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
		~TextureManager() = default;

		static uint32_t LoadTexture(const std::string& path);
		static uint32_t LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths);
	private:
		TextureManager();

		uint32_t LoadTextureImpl(const std::string& path);
		uint32_t LoadTextureMapsImpl(const std::vector<std::pair<std::string, TextureType>>& texturePaths);
	private:
		std::vector<std::unique_ptr<Texture>> s_Textures;
		std::shared_ptr<ShaderStorageBuffer> m_SSBO;

		uint32_t m_Count = 0;
		std::unordered_map<std::string, uint32_t> s_TextureMap;

		static std::unique_ptr<TextureManager> s_TextureManager;
	};
}