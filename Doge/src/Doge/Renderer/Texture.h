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

	class Texture2D
	{
	public:
		uint32_t GetShaderStorageIndex() const { return m_Index; }
	protected:
		void SetShaderStorageIndex(uint32_t index) { m_Index = index; }

		virtual uint64_t GetTextureHandle() = 0;
	private:
		static Scope<Texture2D> Create(const std::string& texturePath);
		static Scope<Texture2D> CreateWhiteTexture();
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

		static Ref<Texture2D> LoadTexture(const std::string& path);
		static Ref<Texture2D> LoadTextureMaps(const std::vector<std::pair<std::string, TextureType>>& texturePaths);

		static Ref<CubemapTexture> LoadCubemap(const std::string& folderPath,
			const std::string& rightFace,
			const std::string& leftFace,
			const std::string& topFace,
			const std::string& bottomFace,
			const std::string& frontFace,
			const std::string& backFace);
	private:
		Ref<Texture2D> LoadTextureImpl(const std::string& path);
		Ref<Texture2D> LoadTextureMapsImpl(const std::vector<std::pair<std::string, TextureType>>& texturePaths);

		Ref<CubemapTexture> LoadCubemapImpl(const std::string& folderPath, const std::array<std::string, 6>& cubemapFaces);
	private:
		Scope<ShaderStorageBuffer> m_SSBO;

		std::unordered_map<std::string, WeakRef<Texture2D>> m_TextureMap;
		std::unordered_map<std::string, WeakRef<CubemapTexture>> m_CubemapMap;

		static uint32_t s_TextureCount;
		static Scope<TextureManager> s_TextureManager;

		static constexpr uint32_t MAX_TEXTURES = 1024;
	};
}