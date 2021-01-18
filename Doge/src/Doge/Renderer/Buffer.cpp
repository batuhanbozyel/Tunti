#include "pch.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBuffer.h"

#include "Buffer.h"
#include "Renderer.h"

namespace Doge
{
	template<typename BufferType T, typename ... Args>
	static Buffer<T> BufferManager::AllocateBuffer(Args&& ... args)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return Buffer<T>();
			case RendererAPI::OpenGL: return OpenGLBufferManager::GetInstance()->AllocateBuffer(std::forward(args));
		}

		LOG_ASSERT(false, "RendererAPI initialization failed!");
		return Buffer<T>();
	}
}