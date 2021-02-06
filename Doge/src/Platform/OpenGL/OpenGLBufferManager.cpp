#include "pch.h"
#include <glad/glad.h>
#include "OpenGLBufferManager.h"

namespace Doge
{
	// OpenGLBufferManager

	MeshData OpenGLBufferManager::AllocateGraphicsBuffer(const Mesh& mesh, size_t key)
	{
		MeshData meshData;
		meshData.Count = mesh.Indices.size();

		uint32_t vertexCount = mesh.Position.size();
		uint32_t indexCount = mesh.Indices.size();

		const auto& cacheIt = m_GraphicBuffersCache.find(key);
		if (cacheIt == m_GraphicBuffersCache.end())
		{
			// Create Vertex Buffer and Index Buffer
			OpenGLGraphicsBuffer openGLBuffer;
			openGLBuffer.VertexCount = vertexCount;
			openGLBuffer.IndexCount = indexCount;

			meshData.BaseVertex = 0;
			meshData.BaseIndex = 0;
			
			glCreateBuffers(1, &openGLBuffer.VertexBuffer);
			glNamedBufferStorage(openGLBuffer.VertexBuffer, vertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

			// Copy Vertex Data into the Buffer
			{
				void* buffPtr = glMapNamedBuffer(openGLBuffer.VertexBuffer, GL_WRITE_ONLY);
				memcpy(buffPtr, mesh.Position.data(), mesh.Position.size() * sizeof(glm::vec4));
				buffPtr = static_cast<int*>(buffPtr) + mesh.Position.size() * 4;

				memcpy(buffPtr, mesh.Normal.data(), mesh.Normal.size() * sizeof(glm::vec3));
				buffPtr = static_cast<int*>(buffPtr) + mesh.Normal.size() * 3;

				memcpy(buffPtr, mesh.TexCoord.data(), mesh.TexCoord.size() * sizeof(glm::vec2));
				buffPtr = static_cast<int*>(buffPtr) + mesh.TexCoord.size() * 2;

				memcpy(buffPtr, mesh.TexIndex.data(), mesh.TexIndex.size() * sizeof(uint32_t));
				glUnmapNamedBuffer(openGLBuffer.VertexBuffer);
			}

			// Create Index Buffer
			glCreateBuffers(1, &openGLBuffer.IndexBuffer);
			glNamedBufferStorage(openGLBuffer.IndexBuffer, openGLBuffer.IndexCount * sizeof(uint32_t), mesh.Indices.data(), GL_DYNAMIC_STORAGE_BIT);

			m_GraphicBuffersCache.insert(m_GraphicBuffersCache.end(), { key, openGLBuffer });

			return meshData;
		}
		else
		{
			// Get Existing Vertex Buffer and Index Buffer
			OpenGLGraphicsBuffer& openGLBufferRef = cacheIt->second;

			// Calculate target sizes of Vertex Buffer and Index Buffer
			vertexCount += openGLBufferRef.VertexCount;
			indexCount += openGLBufferRef.IndexCount;

			meshData.BaseVertex = openGLBufferRef.VertexCount;
			meshData.BaseIndex = openGLBufferRef.IndexCount;

			// Reallocation buffers
			GLuint vertexBuffer;
			GLuint indexBuffer;

			glCreateBuffers(1, &vertexBuffer);
			glNamedBufferStorage(vertexBuffer, vertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

			// Copy Vertex Data into the Buffer
			{
				void* oldBuffPtr = glMapNamedBuffer(openGLBufferRef.VertexBuffer, GL_READ_ONLY);
				void* buffPtr = glMapNamedBuffer(vertexBuffer, GL_WRITE_ONLY);

				// Copy Position data
				memcpy(buffPtr, oldBuffPtr, openGLBufferRef.VertexCount * sizeof(glm::vec4));
				oldBuffPtr = static_cast<int*>(oldBuffPtr) + openGLBufferRef.VertexCount * 4;
				buffPtr = static_cast<int*>(buffPtr) + openGLBufferRef.VertexCount * 4;
				memcpy(buffPtr, mesh.Position.data(), mesh.Position.size() * sizeof(glm::vec4));
				buffPtr = static_cast<int*>(buffPtr) + mesh.Position.size() * 4;

				// Copy Normal data
				memcpy(buffPtr, oldBuffPtr, openGLBufferRef.VertexCount * sizeof(glm::vec3));
				oldBuffPtr = static_cast<int*>(oldBuffPtr) + openGLBufferRef.VertexCount * 3;
				buffPtr = static_cast<int*>(buffPtr) + openGLBufferRef.VertexCount * 3;
				memcpy(buffPtr, mesh.Normal.data(), mesh.Normal.size() * sizeof(glm::vec3));
				buffPtr = static_cast<int*>(buffPtr) + mesh.Normal.size() * 3;

				// Copy TexCoord data
				memcpy(buffPtr, oldBuffPtr, openGLBufferRef.VertexCount * sizeof(glm::vec2));
				oldBuffPtr = static_cast<int*>(oldBuffPtr) + openGLBufferRef.VertexCount * 2;
				buffPtr = static_cast<int*>(buffPtr) + openGLBufferRef.VertexCount * 2;
				memcpy(buffPtr, mesh.TexCoord.data(), mesh.TexCoord.size() * sizeof(glm::vec2));
				buffPtr = static_cast<int*>(buffPtr) + mesh.TexCoord.size() * 2;

				// Copy TexIndex data
				memcpy(buffPtr, oldBuffPtr, openGLBufferRef.VertexCount * sizeof(uint32_t));
				oldBuffPtr = static_cast<int*>(oldBuffPtr) + openGLBufferRef.VertexCount;
				buffPtr = static_cast<int*>(buffPtr) + openGLBufferRef.VertexCount;
				memcpy(buffPtr, mesh.TexIndex.data(), mesh.TexIndex.size() * sizeof(uint32_t));

				glUnmapNamedBuffer(vertexBuffer);
				glUnmapNamedBuffer(openGLBufferRef.VertexBuffer);
			}

			// Create Index Buffer
			glCreateBuffers(1, &indexBuffer);
			glNamedBufferStorage(indexBuffer, indexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
			glCopyNamedBufferSubData(openGLBufferRef.IndexBuffer, indexBuffer, 0, 0, openGLBufferRef.IndexCount * sizeof(uint32_t));
			glNamedBufferSubData(indexBuffer, openGLBufferRef.IndexCount * sizeof(uint32_t), mesh.Indices.size() * sizeof(uint32_t), mesh.Indices.data());

			// Delete old Buffers
			glDeleteBuffers(1, &openGLBufferRef.VertexBuffer);
			glDeleteBuffers(1, &openGLBufferRef.IndexBuffer);

			// Update Buffers and necessary data
			openGLBufferRef.VertexBuffer = vertexBuffer;
			openGLBufferRef.IndexBuffer = indexBuffer;
			openGLBufferRef.VertexCount = vertexCount;
			openGLBufferRef.IndexCount = indexCount;

			return meshData;
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