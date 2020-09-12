#include "pch.h"
#include "OpenGLRendererCommands.h"
#include <glad/glad.h>

namespace Doge
{

	void OpenGLRendererCommands::DrawIndexedImpl(uint32_t count) const
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererCommands::DisableFaceCullingImpl() const
	{
		glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererCommands::EnableFaceCullingImpl(const CullFace& face) const
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT + static_cast<int>(face));
	}

	void OpenGLRendererCommands::SetViewportImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererCommands::ClearColorImpl(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererCommands::ClearImpl() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}