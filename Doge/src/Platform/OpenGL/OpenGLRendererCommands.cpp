#include "pch.h"
#include "OpenGLRendererCommands.h"
#include <glad/glad.h>

namespace Doge
{

	void OpenGLRendererCommands::APIDrawIndexed(uint32_t count) const
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererCommands::APISetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererCommands::APIClearColor(const glm::vec4& color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererCommands::APIClear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}