#pragma once
#include "Doge/Renderer/Context.h"

namespace Doge
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* window);
		~OpenGLContext();
	};
}