#include "pch.h"
#include "Context.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Doge
{
	Scope<Context> Context::Create(void* window)
	{
		switch (Renderer::API)
		{
			case RendererAPI::None:		LOG_ASSERT(false, "RendererAPI is not specified!"); return nullptr;
			case RendererAPI::OpenGL:	return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return nullptr;
	}
}