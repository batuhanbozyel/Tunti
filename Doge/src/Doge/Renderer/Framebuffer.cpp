#include "pch.h"
#include <glad/glad.h>
#include "Framebuffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Doge
{
	constexpr uint32_t MaxFramebufferSize = 8192;

	std::unique_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
		case RendererAPI::OpenGL: return std::make_unique<OpenGLFramebuffer>(specification);
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
		{
			LOG_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		if (m_Specification.Width == width && m_Specification.Height == height) return;

		m_Specification.Width = width;
		m_Specification.Height = height;

		Construct();
	}
}