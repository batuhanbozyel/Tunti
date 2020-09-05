#include "pch.h"
#include "Context.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Doge
{
	std::unique_ptr<Context> Context::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None:		LOG_ASSERT(false, ""); return nullptr;
		case RendererAPI::OpenGL:	return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		LOG_ASSERT(false, "");
		return nullptr;
	}
}