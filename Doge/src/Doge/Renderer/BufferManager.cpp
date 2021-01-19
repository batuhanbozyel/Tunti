#include "pch.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBufferManager.h"

#include "Renderer.h"
#include "BufferManager.h"
#include "Doge/Utility/Mesh.h"

namespace Doge
{

	GraphicsBuffer BufferManager::AllocateGraphicsBuffer(const Mesh& mesh, size_t key)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return GraphicsBuffer();
			case RendererAPI::OpenGL: return OpenGLBufferManager::GetInstance()->AllocateGraphicsBuffer(mesh, key);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return GraphicsBuffer();
	}

}