#include "tpch.h"
#include "GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Tunti
{
	GraphicsContext* GraphicsContext::Create(int width, int height, const char* title, void* monitor, void* share)
	{
		switch (Renderer::CurrentAPI())
		{
			case RendererAPI::None:     T_CORE_ASSERT(false, "GraphicsContext: Renderer API is not specified!"); return nullptr;
			case RendererAPI::OpenGL:   return new OpenGLContext(width, height, title, (GLFWmonitor*)monitor, (GLFWwindow*)share);
		}
		T_CORE_ASSERT(false, "GraphicsContext: Unknown Renderer API specified!");
		return nullptr;
	}

}