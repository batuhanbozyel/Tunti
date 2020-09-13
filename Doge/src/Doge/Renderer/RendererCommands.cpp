#include "pch.h"
#include "RendererCommands.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRendererCommands.h"

namespace Doge
{
	std::unique_ptr<RendererCommands> RendererCommands::s_RendererAPI = nullptr;

	void RendererCommands::Init()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:		LOG_ASSERT(false, "RendererAPI is not specified!");
									s_RendererAPI = nullptr;
									return;
		case RendererAPI::OpenGL:	s_RendererAPI = std::make_unique<Doge::OpenGLRendererCommands>();
									return;
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
	}

	void RendererCommands::DrawIndexed(uint32_t count)
	{
		s_RendererAPI->DrawIndexedImpl(count);
	}

	void RendererCommands::DisableMSAA()
	{
		s_RendererAPI->DisableMSAAImpl();
	}

	void RendererCommands::EnableMSAA()
	{
		s_RendererAPI->EnableMSAAImpl();
	}

	void RendererCommands::DisableDepthTest()
	{
		s_RendererAPI->DisableDepthTestImpl();
	}

	void RendererCommands::EnableDepthTest()
	{
		s_RendererAPI->EnableDepthTestImpl();
	}

	void RendererCommands::DisableFaceCulling()
	{
		s_RendererAPI->DisableFaceCullingImpl();
	}

	void RendererCommands::EnableFaceCulling()
	{
		s_RendererAPI->EnableFaceCullingImpl();
	}

	void RendererCommands::SetFaceCulling(const CullFace& face)
	{
		s_RendererAPI->SetFaceCullingImpl(face);
	}

	void RendererCommands::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewportImpl(x, y, width, height);
	}

	void RendererCommands::ClearColor(const glm::vec4& color)
	{
		s_RendererAPI->ClearColorImpl(color);
	}

	void RendererCommands::Clear()
	{
		s_RendererAPI->ClearImpl();
	}
}

