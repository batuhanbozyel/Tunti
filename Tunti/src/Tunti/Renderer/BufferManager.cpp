#include "pch.h"

#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLBuffer.h"

#include "Renderer.h"
#include "BufferManager.h"

namespace Tunti
{
	MeshBuffer BufferManager::AllocateMeshBufferWithKey(size_t key)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return MeshBuffer();
			case RendererAPI::OpenGL: return OpenGLBufferCache::GetInstance()->AllocateMeshBufferWithKey(key);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return MeshBuffer();
	}

	SubmeshBuffer BufferManager::AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& data)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: LOG_ASSERT(false, "RendererAPI is not specified!"); return SubmeshBuffer();
			case RendererAPI::OpenGL: return OpenGLBufferCache::GetInstance()->AllocateSubmeshFromMeshBuffer(mesh, data);
		}

		LOG_ASSERT(false, "RendererAPI is not specified!");
		return SubmeshBuffer();
	}
}