#pragma once
#include "Doge/Renderer/Framebuffer.h"

namespace Doge
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification);
		~OpenGLFramebuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		virtual void Construct() override;
	private:
		uint32_t m_RendererID;
	};
}