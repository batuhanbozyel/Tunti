#pragma once
#include "Doge/Core/Base.h"

namespace Doge
{
	class ShaderStorageBuffer;
	class TextureManager;

	enum class TextureType {
		Diffuse,
		Specular,
		COUNT
	};

	static uint32_t OffsetofTextureType(TextureType type)
	{
		return static_cast<uint32_t>(type) * sizeof(uint64_t);
	}

	static constexpr uint32_t SizeofTextureMap = static_cast<uint32_t>(TextureType::COUNT) * sizeof(uint64_t);

	class Texture
	{
	public:
		uint32_t GetShaderStorageIndex() const { return m_Index; }
	protected:
		void SetShaderStorageIndex(uint32_t index) { m_Index = index; }

		virtual uint64_t GetTextureHandle() = 0;
	private:
		static Scope<Texture> Create(const std::string& texturePath);
		static Scope<Texture> CreateWhiteTexture();
	private:
		uint32_t m_Index = 0;

		friend class TextureManager;
	};

	class CubemapTexture
	{
	public:
		virtual void Bind(uint32_t slot) const = 0;
	private:
		static Scope<CubemapTexture> Create(const std::array<std::string, 6>& CubemapFiles);

		friend class TextureManager;
	};

	class TextureManager
	{
	public:
		static void Init();
		TextureManager();
		~TextureManager() = default;

		static Ref<Texture> LoadTexture(const std::string& path);
		static Ref<Texture> LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths);

		static Ref<CubemapTexture> LoadCubemap(const std::string& folderPath,
			const std::string& PosXFilename,
			const std::string& NegXFilename,
			const std::string& PosYFilename,
			const std::string& NegYFilename,
			const std::string& PosZFilename,
			const std::string& NegZFilename);
	private:
		Ref<Texture> LoadTextureImpl(const std::string& path);
		Ref<Texture> LoadTextureMapsImpl(const std::vector<std::pair<std::string, TextureType>>& texturePaths);

		Ref<CubemapTexture> LoadCubemapImpl(const std::string& folderPath, const std::array<std::string, 6>& cubemapFaces);
	private:
		Scope<ShaderStorageBuffer> m_SSBO;

		std::unordered_map<std::string, Ref<Texture>> m_TextureMap;
		std::unordered_map<std::string, Ref<CubemapTexture>> m_CubemapMap;

		static uint32_t s_TextureCount;
		static Scope<TextureManager> s_TextureManager;

		static constexpr uint32_t MAX_TEXTURES = 1024;
	};
}