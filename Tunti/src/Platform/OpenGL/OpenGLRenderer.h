#pragma once
#include "Tunti/Renderer/Renderer.h"

struct GLFWwindow;

namespace Tunti
{
	class OpenGLShaderProgram;

	class OpenGLRenderer final : public RendererAPI
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
	private:
		void InitDeferredRenderer(GLFWwindow* window);
		void DestroyDeferredRenderer();
	private:
		static void ConstructScreenBuffer(uint32_t width, uint32_t height);
		static void ConstructGBuffer();
		static void ConstructShadowMapBuffer(uint32_t resolution);

		static void SetCommonUniformProperties(const Ref<Material>& material, const Ref<OpenGLShaderProgram>& shader);
		static void SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance, const Ref<OpenGLShaderProgram>& shader);
	};
}