#include "tpch.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLRendererCommand.h"

namespace Tunti
{
	// Default API is set to OpenGL
	RendererAPI Renderer::s_RendererAPI = RendererAPI::OpenGL;
	RendererCommand* Renderer::s_RendererCommand = nullptr;

	void Renderer::Init()
	{
		switch (Renderer::CurrentAPI())
		{
			case RendererAPI::None:     T_CORE_ASSERT(false, "Shader: Renderer API is not specified!");
			case RendererAPI::OpenGL:   s_RendererCommand = new OpenGLRendererCommand();
		}
	}

	void Renderer::ClearColor(const glm::vec4& color)
	{
		s_RendererCommand->ClearColor(color);
	}

	void Renderer::Clear()
	{
		s_RendererCommand->Clear();
	}
}