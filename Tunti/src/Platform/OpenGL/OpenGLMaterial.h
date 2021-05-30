#pragma once
#include "Tunti/Renderer/Material.h"

namespace Tunti
{
	class OpenGLMaterialInstance;

	class OpenGLMaterial
	{
	public:
		explicit OpenGLMaterial(const std::initializer_list<MaterialProperty>& props, uint32_t cacheIdx);
		~OpenGLMaterial();

		Ref<MaterialInstance> CreateInstance();

		void Bind() const;

		void SetValue(uint32_t offset, uint32_t size, const void* value) const;
	private:
		uint32_t m_Buffer;
		uint32_t m_InstanceCount = 0;
		Ref<Material> m_Material;
		Ref<MaterialInstance> m_DefaultInstance;

		friend class OpenGLMaterialCache;
	};

	class OpenGLMaterialCache final
	{
	public:
		~OpenGLMaterialCache() = default;

		Ref<Material> PBRMaterial();
		Ref<Material> CreateMaterial(const std::initializer_list<MaterialProperty>& props);
		Ref<MaterialInstance> GetDefaultInstanceFrom(const Ref<Material>& material);
		Ref<MaterialInstance> CreateInstanceFrom(const Ref<Material>& material);

		void Flush();

		static OpenGLMaterialCache* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLMaterialCache;

			return s_Instance;
		}

		const OpenGLMaterial& operator[](uint32_t index) const
		{
			return *m_MaterialsCache[index];
		}

		OpenGLMaterialCache(const OpenGLMaterialCache& other) = delete;
		OpenGLMaterialCache& operator=(const OpenGLMaterialCache& other) = delete;
	private:
		explicit OpenGLMaterialCache() = default;
	private:
		std::vector<Scope<OpenGLMaterial>> m_MaterialsCache;

		static inline OpenGLMaterialCache* s_Instance = nullptr;
	};
}