#include "pch.h"
#include "OpenGLFramebuffer.h"
#include <glad/glad.h>

namespace Doge
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification)
	{
		m_Specification = specification;
		Construct();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteRenderbuffers(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::BlitMultisampled() const
	{
		glBlitNamedFramebuffer(m_RendererID, 0, 0, 0, m_Specification.Width, m_Specification.Height, 0, 0, m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::BindColorAttachment(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_ColorAttachment);
	}

	void OpenGLFramebuffer::Construct()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteRenderbuffers(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_RendererID);

		// Construct Color Buffer Texture
		if (m_Specification.SwapChainTarget)
		{
			glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
			glTextureStorage2DMultisample(m_ColorAttachment, m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, GL_TRUE);
		}
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
			glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(m_ColorAttachment, 1, GL_RGBA8, m_Specification.Width, m_Specification.Height);
		}
		
		// Set Framebuffer's Color Attachment
		glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0, m_ColorAttachment, 0);

		// Construct Depth Render Buffer
		glCreateRenderbuffers(1, &m_DepthAttachment);

		if (m_Specification.SwapChainTarget)
			glNamedRenderbufferStorageMultisample(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		else
			glNamedRenderbufferStorage(m_DepthAttachment, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		
		// Set Framebuffer's Depth Attachment
		glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

		LOG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");
	}
}