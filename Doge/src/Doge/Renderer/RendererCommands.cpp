#include "pch.h"
#include "RendererCommands.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRendererCommands.h"

namespace Doge
{
	std::shared_ptr<Doge::RendererAPICommands> RendererCommands::s_RendererAPI = nullptr;

	void RendererCommands::Init()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:		LOG_ASSERT(false, "RendererAPI is not specified!");
									s_RendererAPI = nullptr;
									return;
		case RendererAPI::OpenGL:	s_RendererAPI = std::make_shared<Doge::OpenGLRendererCommands>();
									return;
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
	}

	void RendererCommands::DrawIndexed(uint32_t count)
	{
		s_RendererAPI->APIDrawIndexed(count);
	}

	void RendererCommands::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		s_RendererAPI->APISetViewport(x, y, width, height);
	}

	void RendererCommands::ClearColor(const glm::vec4& color)
	{
		s_RendererAPI->APIClearColor(color);
	}

	void RendererCommands::Clear()
	{
		s_RendererAPI->APIClear();
	}
}

