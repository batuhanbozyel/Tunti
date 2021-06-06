#include "pch.h"
#include <glad/glad.h>
#include "OpenGLBuffer.h"

#include "Tunti/Renderer/RendererBindingTable.h"

namespace Tunti
{
	// OpenGLMeshBuffer
	 
	OpenGLMeshBuffer::OpenGLMeshBuffer(const Submesh& submesh)
		: m_VertexCount(submesh.Positions.size()), m_IndexCount(submesh.Indices.size())
	{
		glCreateBuffers(1, &m_VertexBuffer);
		glCreateBuffers(1, &m_MaterialIndexBuffer);
		glCreateBuffers(1, &m_IndexBuffer);

		glNamedBufferStorage(m_VertexBuffer, submesh.Positions.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
		{
			float* buffPtr = (float*)glMapNamedBuffer(m_VertexBuffer, GL_WRITE_ONLY);
			memcpy(buffPtr, submesh.Positions.data(), m_VertexCount * sizeof(glm::vec3));
			buffPtr += submesh.Positions.size() * 3;

			memcpy(buffPtr, submesh.Normals.data(), m_VertexCount * sizeof(glm::vec3));
			buffPtr += submesh.Normals.size() * 3;

			memcpy(buffPtr, submesh.TexCoords.data(), m_VertexCount * sizeof(glm::vec2));
			glUnmapNamedBuffer(m_VertexBuffer);
		}
		glNamedBufferStorage(m_MaterialIndexBuffer, m_VertexCount * sizeof(uint32_t), submesh.MaterialIndices.data(), GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(m_IndexBuffer, m_IndexCount * sizeof(uint32_t), submesh.Indices.data(), GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLMeshBuffer::~OpenGLMeshBuffer()
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteBuffers(1, &m_MaterialIndexBuffer);
	}

	void OpenGLMeshBuffer::Bind() const
	{
		GLuint buffers[2] = { m_MaterialIndexBuffer, m_VertexBuffer };
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, RendererBindingTable::MaterialIndexBufferShaderStorageBuffer, 2, buffers);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	}

	SubmeshBuffer OpenGLMeshBuffer::AllocateSubmesh(const Submesh& submesh)
	{
		SubmeshBuffer submeshBuffer;
		submeshBuffer.BaseVertex = m_VertexCount;
		submeshBuffer.BaseIndex = m_IndexCount;
		submeshBuffer.Count = submesh.Indices.size();

		// Calculate target sizes of Vertex Buffer and Index Buffer
		uint32_t vertexCount = submesh.Positions.size() + m_VertexCount;
		uint32_t indexCount = submesh.Indices.size() + m_IndexCount;

		// Reallocation buffers
		GLuint materialIndexBuffer;
		GLuint vertexBuffer;
		GLuint indexBuffer;

		glCreateBuffers(1, &vertexBuffer);
		glNamedBufferStorage(vertexBuffer, vertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

		// Copy Vertex Data into the Buffer
		{
			void* oldBuffPtr = glMapNamedBuffer(m_VertexBuffer, GL_READ_ONLY);
			void* buffPtr = glMapNamedBuffer(vertexBuffer, GL_WRITE_ONLY);

			// Copy Position data
			memcpy(buffPtr, oldBuffPtr, m_VertexCount * sizeof(glm::vec3));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + m_VertexCount * 3;
			buffPtr = static_cast<int*>(buffPtr) + m_VertexCount * 3;
			memcpy(buffPtr, submesh.Positions.data(), submesh.Positions.size() * sizeof(glm::vec3));
			buffPtr = static_cast<int*>(buffPtr) + submesh.Positions.size() * 3;

			// Copy Normal data
			memcpy(buffPtr, oldBuffPtr, m_VertexCount * sizeof(glm::vec3));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + m_VertexCount * 3;
			buffPtr = static_cast<int*>(buffPtr) + m_VertexCount * 3;
			memcpy(buffPtr, submesh.Normals.data(), submesh.Normals.size() * sizeof(glm::vec3));
			buffPtr = static_cast<int*>(buffPtr) + submesh.Normals.size() * 3;

			// Copy TexCoord data
			memcpy(buffPtr, oldBuffPtr, m_VertexCount * sizeof(glm::vec2));
			oldBuffPtr = static_cast<int*>(oldBuffPtr) + m_VertexCount * 2;
			buffPtr = static_cast<int*>(buffPtr) + m_VertexCount * 2;
			memcpy(buffPtr, submesh.TexCoords.data(), submesh.TexCoords.size() * sizeof(glm::vec2));

			glUnmapNamedBuffer(vertexBuffer);
			glUnmapNamedBuffer(m_VertexBuffer);
		}

		// Create Material Index Buffer
		glCreateBuffers(1, &materialIndexBuffer);
		glNamedBufferStorage(materialIndexBuffer, vertexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glCopyNamedBufferSubData(m_MaterialIndexBuffer, materialIndexBuffer, 0, 0, m_VertexCount * sizeof(uint32_t));
		glNamedBufferSubData(materialIndexBuffer, m_VertexCount * sizeof(uint32_t), submesh.MaterialIndices.size() * sizeof(uint32_t), submesh.MaterialIndices.data());

		// Create Index Buffer
		glCreateBuffers(1, &indexBuffer);
		glNamedBufferStorage(indexBuffer, indexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glCopyNamedBufferSubData(m_IndexBuffer, indexBuffer, 0, 0, m_IndexCount * sizeof(uint32_t));
		glNamedBufferSubData(indexBuffer, m_IndexCount * sizeof(uint32_t), submesh.Indices.size() * sizeof(uint32_t), submesh.Indices.data());

		// Delete old Buffers
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_MaterialIndexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);

		// Update Buffers and necessary data
		m_MaterialIndexBuffer = materialIndexBuffer;
		m_VertexBuffer = vertexBuffer;
		m_IndexBuffer = indexBuffer;
		m_VertexCount = vertexCount;
		m_IndexCount = indexCount;

		return submeshBuffer;
	}

	// OpenGLBufferManager

	MeshBuffer OpenGLBufferCache::AllocateMeshBufferWithKey(size_t key)
	{
		MeshBuffer buffer;
		buffer.Key = key;
		auto& meshBufferArray = m_MeshBuffersCache[key];
		buffer.Index = meshBufferArray.size();
		meshBufferArray.push_back(nullptr);
		return buffer;
	}

	SubmeshBuffer OpenGLBufferCache::AllocateSubmeshFromMeshBuffer(MeshBuffer& mesh, const Submesh& submesh)
	{
		auto& meshBufferCacheIt = m_MeshBuffersCache.find(mesh.Key);
		if (meshBufferCacheIt != m_MeshBuffersCache.end())
		{
			auto& meshBuffer = meshBufferCacheIt->second[mesh.Index];
			if (meshBuffer == nullptr)
			{
				meshBuffer = std::move(CreateScope<OpenGLMeshBuffer>(submesh));
				mesh.IsValid = true;
				SubmeshBuffer submeshBuffer;
				submeshBuffer.Count = submesh.Indices.size();
				return submeshBuffer;
			}
			return meshBuffer->AllocateSubmesh(submesh);
		}

		return SubmeshBuffer();
	}

	void OpenGLBufferCache::Flush()
	{
		m_MeshBuffersCache.clear();
	}
}