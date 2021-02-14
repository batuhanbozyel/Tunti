#pragma once
#include "Tunti/Renderer/Renderer.h"

struct GLFWwindow;

namespace Tunti
{
	class OpenGLShader;

	class OpenGLRenderer final : public RendererAPI
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
	private:
		void InitDeferredRenderer(GLFWwindow* window);
		void DestroyDeferredRenderer();
	private:
		static void ConstructGBuffer(uint32_t width, uint32_t height);

		static void SetCommonUniformProperties(const Ref<Material>& material, const Ref<OpenGLShader>& shader);
		static void SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance, const Ref<OpenGLShader>& shader);
	};
}