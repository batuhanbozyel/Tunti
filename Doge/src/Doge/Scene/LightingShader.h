#pragma once

namespace Doge
{
	class Shader;
	class ShaderLibrary;

	class LightingShader
	{
	public:
		static Ref<Shader> PhongLighting()
		{
			return ShaderLibrary::CreateShader("../Doge/assets/shaders/PhongLighting.glsl");
		}
	};
}