#include "pch.h"
#include "Renderer.h"

#include "Tunti/Core/Application.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Tunti
{
	void Renderer::Init()
	{
		std::call_once(s_Context, []()
		{
			switch (Application::GetRenderAPI())
			{
				case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); break;
				case RenderAPI::OpenGL: s_Renderer = new OpenGLRenderer; break;
			}

			Log::Trace("Renderer has initialized successfully!");
		});
	}

	void Renderer::Destroy()
	{
		if (s_RenderPipeline)
		{
			delete s_RenderPipeline;
		}

		if (s_Renderer)
		{
			delete s_Renderer;
		}
	}

	void Renderer::OutputToScreenFramebuffer(Texture2D texture)
	{
		s_Renderer->OutputToScreenFramebufferImpl(texture);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RenderPipeline->OnWindowResize(width, height);
	}

	void Renderer::SetRenderPipeline(ScriptableRenderPipeline* renderPipeline)
	{
		if (s_RenderPipeline)
		{
			delete s_RenderPipeline;
		}

		s_RenderPipeline = renderPipeline;
	}
}