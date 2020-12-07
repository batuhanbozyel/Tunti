#pragma once
#include "Doge/Renderer/Framebuffer.h"

namespace Doge
{
	class OpenGLFramebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification);
		~OpenGLFramebuffer();

		void BlitMultisampled(const OpenGLFramebuffer& target) const;
		void Resize(uint32_t width, uint32_t height);

		void Bind() const;
		void Unbind() const;

		void BindColorAttachment(uint32_t slot) const;

		inline uint32_t GetColorAttachment() const { return m_ColorAttachment; }
		inline uint32_t GetViewportWidth() const { return m_Specification.Width; }
		inline uint32_t GetViewportHeight() const { return m_Specification.Height; }
	private:
		void Construct();
	private:
		GLuint m_FramebufferHandle;
		GLuint m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};
}