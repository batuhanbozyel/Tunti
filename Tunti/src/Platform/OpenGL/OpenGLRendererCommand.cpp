#include "tpch.h"
#include "OpenGLRendererCommand.h"

#include <glad/glad.h>

namespace Tunti
{
	void OpenGLRendererCommand::ClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

}