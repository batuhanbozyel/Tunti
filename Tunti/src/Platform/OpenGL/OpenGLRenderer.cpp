#include "pch.h"
#include <glad/glad.h>

#include "OpenGLRenderer.h"
#include "OpenGLShader.h"
#include "OpenGLDrawParams.h"
#include "OpenGLDeferredRenderPipeline.h"

#include "Tunti/Core/Application.h"

namespace Tunti
{
	static void OpenGLMessageCallback(
		unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:
			{
				Log::Critical(message);
				LOG_ASSERT(false);
				return;
			}
			case GL_DEBUG_SEVERITY_MEDIUM:       Log::Error(message); return;
			case GL_DEBUG_SEVERITY_LOW:          Log::Warn(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: Log::Trace(message); return;
		}
	}

	OpenGLRenderer::OpenGLRenderer()
	{
		glfwMakeContextCurrent(Application::GetWindow()->GetHandle());
		glfwSwapInterval(static_cast<int>(Application::GetWindow()->GetWindowProps().VSync));

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOG_ASSERT(status, "Glad initialization failed");

#ifdef DEBUG_ENABLED
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		LOG_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Tunti Engine requires at least OpenGL version 4.5!");

		Log::Info(glGetString(GL_RENDERER));
		Log::Info(glGetString(GL_VERSION));

		Log::Trace("Context creation succeed!");

		m_ScreenOutputShader = OpenGLShaderCache::GetInstance()->LoadShaderProgram(RendererShaders::FullscreenTriangle);
		m_ScreenOutputShader->SetUniformInt("u_Texture", RendererBindingTable::EmptyTextureUnitBase);
		SetRenderPipeline(new OpenGLDeferredRenderPipeline());
	}

	void OpenGLRenderer::OutputToScreenFramebufferImpl(Texture2D texture) const
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);

		glBindTextureUnit(RendererBindingTable::EmptyTextureUnitBase, texture);
		m_ScreenOutputShader->Bind();
		glDrawArraysIndirect(GL_TRIANGLE_STRIP, &QuadIndirectParams);
	}
}