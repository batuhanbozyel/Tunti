#pragma once
#include "Tunti/Renderer/Renderer.h"

namespace Tunti
{
	class OpenGLShaderProgram;

	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer() = default;

		virtual void OutputToScreenFramebufferImpl(Texture2D texture) const override;
	private:
		Ref<OpenGLShaderProgram> m_ScreenOutputShader;
	};
}