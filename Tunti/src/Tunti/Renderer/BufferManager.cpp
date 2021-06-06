#include "pch.h"
#include "BufferManager.h"
#include "Renderer.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Tunti/Core/Application.h"

namespace Tunti
{
	MeshBuffer BufferManager::AllocateMeshBufferWithKey(size_t key)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return MeshBuffer();
			case RenderAPI::OpenGL: return OpenGLBufferCache::GetInstance()->AllocateMeshBufferWithKey(key);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return MeshBuffer();
	}

	SubmeshBuffer BufferManager::AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& data)
	{
		switch (Application::GetRenderAPI())
		{
			case RenderAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return SubmeshBuffer();
			case RenderAPI::OpenGL: return OpenGLBufferCache::GetInstance()->AllocateSubmeshFromMeshBuffer(mesh, data);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return SubmeshBuffer();
	}
}