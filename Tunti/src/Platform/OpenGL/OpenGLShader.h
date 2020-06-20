#pragma once

#include <string>
#include "Tunti/Renderer/Shader.h"

namespace Tunti
{
	class OpenGLShader : public Shader
	{
	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();
	private:
		uint32_t m_RendererID;
		friend Shader;
	};
}