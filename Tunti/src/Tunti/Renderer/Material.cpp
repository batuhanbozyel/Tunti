#include "pch.h"
#include "Material.h"
#include "Renderer.h"

#include "Tunti/Core/Application.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Tunti
{
	Material::Material(const std::initializer_list<MaterialProperty>& props)
		: Properties(props)
	{

	}

	uint32_t Material::CalculateSize() const
	{
		uint32_t size = 0;
		for (const auto& prop : Properties)
		{
			size += prop.ValueSize();
		}
		return size;
	}

	// MaterialInstance
	
	void MaterialInstance::SetValue(uint32_t propertyIdx, const MaterialPropertyValue& value)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return;
			case RenderAPI::OpenGL:
			{
				if (Properties[propertyIdx].Value != value)
				{
					uint32_t offset = Index * BaseMaterial->CalculateSize();
					for (uint32_t i = 0; i < propertyIdx; i++)
					{
						offset += Properties[i].ShaderAlignmentSize();
					}

					Properties[propertyIdx].Value = value;
					(*OpenGLMaterialCache::GetInstance())[BaseMaterial->Index].SetValue(offset, Properties[propertyIdx].ValueSize(), &value);
				}
				return;
			}
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
	}

	MaterialInstance::MaterialInstance(const Ref<Material>& baseMaterial)
		: BaseMaterial(baseMaterial), Properties(baseMaterial->Properties)
	{

	}

	// MaterialLibrary

	Ref<Material> MaterialLibrary::PBRMaterial()
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case RenderAPI::OpenGL: return OpenGLMaterialCache::GetInstance()->PBRMaterial();
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Ref<Material> MaterialLibrary::CreateMaterial(const std::initializer_list<MaterialProperty>& props)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case RenderAPI::OpenGL: return OpenGLMaterialCache::GetInstance()->CreateMaterial(props);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Ref<MaterialInstance> MaterialLibrary::GetDefaultInstanceFrom(const Ref<Material>& material)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case RenderAPI::OpenGL: return OpenGLMaterialCache::GetInstance()->GetDefaultInstanceFrom(material);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	Ref<MaterialInstance> MaterialLibrary::CreateInstanceFrom(const Ref<Material>& material)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case RenderAPI::OpenGL: return OpenGLMaterialCache::GetInstance()->CreateInstanceFrom(material);
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
}