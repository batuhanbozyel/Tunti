#pragma once
#include <string_view>

namespace Doge
{
	class Shader;
	class ShaderLibrary;

	class RendererBaseShaders
	{
	public:
		static Ref<Shader> ObjectOutlining()
		{
			return ShaderLibrary::CreateShader("../Doge/assets/shaders/ObjectOutlining.glsl");
		}

		static Ref<Shader> Skybox()
		{
			return ShaderLibrary::CreateShader("../Doge/assets/shaders/Skybox.glsl");
		}

		static Ref<Shader> TexturedQuad()
		{
			return ShaderLibrary::CreateShader("../Doge/assets/shaders/TexturedQuad.glsl");
		}		
	};
}