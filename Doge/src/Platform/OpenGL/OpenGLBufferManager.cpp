#include "pch.h"
#include <glad/glad.h>
#include "OpenGLBufferManager.h"
#include "Doge/Utility/Mesh.h"

namespace Doge
{
	// OpenGLBufferManager

	MeshData OpenGLBufferManager::AllocateGraphicsBuffer(const Mesh& mesh, size_t key)
	{
		uint32_t vertexSize = 0;
		uint32_t indexCount = mesh.Indices.size();
		vertexSize += mesh.Position.size() * sizeof(glm::vec4);
		vertexSize += mesh.Normal.size() * sizeof(glm::vec3);
		vertexSize += mesh.TexCoord.size() * sizeof(glm::vec2);
		vertexSize += mesh.TexIndex.size() * sizeof(uint32_t);

		const auto& cacheIt = m_GraphicBuffersCache.find(key);
		if (cacheIt == m_GraphicBuffersCache.end())
		{
			OpenGLGraphicsBuffer openGLBuffer;
			openGLBuffer.VertexSize = vertexSize;
			openGLBuffer.IndexCount = indexCount;

			MeshData buffer;
			buffer.BaseVertex = 0;
			buffer.Count = mesh.Indices.size();

			glCreateBuffers(1, &openGLBuffer.VertexBuffer);
			glNamedBufferStorage(openGLBuffer.VertexBuffer, openGLBuffer.VertexSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

			// Copy Vertex Data into the Buffer
			void* buffPtr = glMapNamedBuffer(openGLBuffer.VertexBuffer, GL_WRITE_ONLY);
			memcpy(buffPtr, mesh.Position.data(), mesh.Position.size() * sizeof(glm::vec4));
			buffPtr = static_cast<int*>(buffPtr) + mesh.Position.size() * 4;

			memcpy(buffPtr, mesh.Normal.data(), mesh.Normal.size() * sizeof(glm::vec3));
			buffPtr = static_cast<int*>(buffPtr) + mesh.Normal.size() * 3;

			memcpy(buffPtr, mesh.TexCoord.data(), mesh.TexCoord.size() * sizeof(glm::vec2));
			buffPtr = static_cast<int*>(buffPtr) + mesh.TexCoord.size() * 2;

			memcpy(buffPtr, mesh.TexIndex.data(), mesh.TexIndex.size() * sizeof(uint32_t));
			glUnmapNamedBuffer(openGLBuffer.VertexBuffer);

			// Create Index Buffer
			glCreateBuffers(1, &openGLBuffer.IndexBuffer);
			glNamedBufferStorage(openGLBuffer.IndexBuffer, openGLBuffer.IndexCount * sizeof(uint32_t), mesh.Indices.data(), GL_DYNAMIC_STORAGE_BIT);

			const auto& insertedBuffer = m_GraphicBuffersCache.insert(m_GraphicBuffersCache.end(), { key, openGLBuffer });
			return buffer;
		}
		else
		{
			OpenGLGraphicsBuffer& openGLBufferRef = cacheIt->second;
			vertexSize += openGLBufferRef.VertexSize;
			indexCount += openGLBufferRef.IndexCount;

			uint32_t vertexCount = openGLBufferRef.VertexSize / (sizeof(Vertex));

			MeshData buffer;
			buffer.BaseVertex = vertexCount;
			buffer.Count = mesh.Indices.size();

			GLuint vertexBuffer;
			GLuint indexBuffer;

			glCreateBuffers(1, &vertexBuffer);
			glNamedBufferStorage(vertexBuffer, vertexSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

			// Copy Vertex Data into the Buffer
			void* oldBuffPtr = glMapNamedBuffer(vertexBuffer, GL_READ_ONLY);
			void* buffPtr = glMapNamedBuffer(vertexBuffer, GL_WRITE_ONLY);

			// Copy Position data
			memcpy(buffPtr, oldBuffPtr, vertexCount * sizeof(glm::vec4));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + vertexCount * 4;
			buffPtr = static_cast<int*>(buffPtr) + vertexCount * 4;
			memcpy(buffPtr, mesh.Position.data(), mesh.Position.size() * sizeof(glm::vec4));
			buffPtr = static_cast<int*>(buffPtr) + mesh.Position.size() * 4;

			// Copy Normal data
			memcpy(buffPtr, oldBuffPtr, vertexCount * sizeof(glm::vec3));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + vertexCount * 3;
			buffPtr = static_cast<int*>(buffPtr) + vertexCount * 3;
			memcpy(buffPtr, mesh.Normal.data(), mesh.Normal.size() * sizeof(glm::vec3));
			buffPtr = static_cast<int*>(buffPtr) + mesh.Normal.size() * 3;

			// Copy TexCoord data
			memcpy(buffPtr, oldBuffPtr, vertexCount * sizeof(glm::vec2));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + vertexCount * 2;
			buffPtr = static_cast<int*>(buffPtr) + vertexCount * 2;
			memcpy(buffPtr, mesh.TexCoord.data(), mesh.TexCoord.size() * sizeof(glm::vec2));
			buffPtr = static_cast<int*>(buffPtr) + mesh.TexCoord.size() * 2;

			// Copy TexIndex data
			memcpy(buffPtr, oldBuffPtr, vertexCount * sizeof(uint32_t));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + vertexCount;
			buffPtr = static_cast<int*>(buffPtr) + vertexCount;
			memcpy(buffPtr, mesh.TexIndex.data(), mesh.TexIndex.size() * sizeof(uint32_t));
			glUnmapNamedBuffer(vertexBuffer);

			// Create Index Buffer
			glCreateBuffers(1, &indexBuffer);
			glNamedBufferStorage(indexBuffer, indexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
			glCopyNamedBufferSubData(openGLBufferRef.IndexBuffer, indexBuffer, 0, 0, openGLBufferRef.IndexCount * sizeof(uint32_t));
			glNamedBufferSubData(indexBuffer, openGLBufferRef.IndexCount * sizeof(uint32_t), mesh.Indices.size(), mesh.Indices.data());

			// Delete old Buffers
			glDeleteBuffers(1, &openGLBufferRef.VertexBuffer);
			glDeleteBuffers(1, &openGLBufferRef.IndexBuffer);
			openGLBufferRef.VertexBuffer = vertexBuffer;
			openGLBufferRef.IndexBuffer = indexBuffer;

			return buffer;
		}
	}

	void OpenGLBufferManager::Flush()
	{
		for (auto& [key, buffer] : m_GraphicBuffersCache)
		{
			glDeleteBuffers(1, &buffer.VertexBuffer);
			glDeleteBuffers(1, &buffer.IndexBuffer);
		}
		m_GraphicBuffersCache.clear();
	}
}