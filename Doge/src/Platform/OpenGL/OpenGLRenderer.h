#pragma once
#include "Doge/Renderer/Renderer.h"

namespace Doge
{
	class OpenGLRenderer final : public RendererAPI
	{
	public:
		OpenGLRenderer(const WindowProps& props);
		~OpenGLRenderer();
	private:
		static void ConstructGBuffer(uint32_t width, uint32_t height);

		static void SetCommonUniformProperties(const Ref<Material>& material);
		static void SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance);
	};
}