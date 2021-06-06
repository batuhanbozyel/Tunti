#pragma once
#include "Tunti/Renderer/BufferManager.h"

namespace Tunti
{
	class OpenGLMeshBuffer
	{
	public:
		OpenGLMeshBuffer(const Submesh& submesh);
		~OpenGLMeshBuffer();

		void Bind() const;

		SubmeshBuffer AllocateSubmesh(const Submesh& data);

		uint32_t GetVertexCount() const { return m_VertexCount; }
	private:
		uint32_t m_VertexCount = 0;
		uint32_t m_IndexCount = 0;

		GLuint m_MaterialIndexBuffer;
		GLuint m_VertexBuffer;
		GLuint m_IndexBuffer;

		friend class OpenGLBufferCache;
	};

	class OpenGLBufferCache
	{
	public:
		~OpenGLBufferCache() = default;

		MeshBuffer AllocateMeshBufferWithKey(size_t key);
		SubmeshBuffer AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& submesh);

		void Flush();

		static OpenGLBufferCache* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLBufferCache;

			return s_Instance;
		}

		const OpenGLMeshBuffer& GetOpenGLMeshBufferWithKey(const MeshBuffer& buffer) const
		{
			const auto& cacheIt = m_MeshBuffersCache.find(buffer.Key);
			LOG_ASSERT(cacheIt != m_MeshBuffersCache.end() && buffer.Index < cacheIt->second.size(), "Buffer does not exist in the cache");
			return *cacheIt->second[buffer.Index];
		}

		OpenGLBufferCache(const OpenGLBufferCache& other) = delete;
		OpenGLBufferCache& operator=(const OpenGLBufferCache& other) = delete;
	private:
		explicit OpenGLBufferCache() = default;
	private:
		std::unordered_map<size_t, std::vector<Scope<OpenGLMeshBuffer>>> m_MeshBuffersCache;

		static inline OpenGLBufferCache* s_Instance = nullptr;
	};
}