#pragma once
#include "Tunti/Renderer/BufferManager.h"

namespace Tunti
{
	struct OpenGLGraphicsBuffer
	{
		GLuint VertexBuffer;
		GLuint IndexBuffer;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
	};

	class OpenGLBufferManager
	{
	public:
		~OpenGLBufferManager() = default;

		MeshData AllocateGraphicsBuffer(const Mesh& mesh, size_t key);

		void Flush();

		static OpenGLBufferManager* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLBufferManager;

			return s_Instance;
		}

		const OpenGLGraphicsBuffer& operator[](size_t key) const
		{
			const auto& cacheIt = m_GraphicBuffersCache.find(key);
			LOG_ASSERT(cacheIt != m_GraphicBuffersCache.end(), "Buffer does not exist in the cache");
			return cacheIt->second;
		}

		OpenGLBufferManager(const OpenGLBufferManager& other) = delete;
		OpenGLBufferManager& operator=(const OpenGLBufferManager& other) = delete;
	private:
		explicit OpenGLBufferManager() = default;
	private:
		std::unordered_map<size_t, OpenGLGraphicsBuffer> m_GraphicBuffersCache;

		static inline OpenGLBufferManager* s_Instance = nullptr;
	};
}