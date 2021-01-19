#pragma once
#include "Doge/Renderer/BufferManager.h"

namespace Doge
{
	struct OpenGLGraphicsBuffer
	{
		GLuint* VertexBuffer;
		GLuint* IndexBuffer;
		uint32_t VertexSize = 0;
		uint32_t IndexCount = 0;
	};

	class OpenGLBufferManager
	{
	public:
		~OpenGLBufferManager() = default;

		GraphicsBuffer AllocateGraphicsBuffer(const Mesh& mesh, size_t key);

		void Flush();

		static OpenGLBufferManager* const GetInstance()
		{
			if (!s_Instance)
				s_Instance = new OpenGLBufferManager;

			return s_Instance;
		}

		OpenGLBufferManager(const OpenGLBufferManager& other) = delete;
		OpenGLBufferManager& operator=(const OpenGLBufferManager& other) = delete;
	private:
		explicit OpenGLBufferManager() = default;
	private:
		std::unordered_map<size_t, OpenGLGraphicsBuffer> m_GraphicBuffersCache;

		static OpenGLBufferManager* s_Instance;
	};
}