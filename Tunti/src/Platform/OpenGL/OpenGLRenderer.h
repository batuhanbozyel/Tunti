#pragma once
#include "Tunti/Renderer/Renderer.h"

namespace Tunti
{
	class OpenGLRenderer final : public RendererAPI
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
	private:
		static void ConstructGBuffer(uint32_t width, uint32_t height);

		static void SetCommonUniformProperties(const Ref<Material>& material);
		static void SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance);
	};
}