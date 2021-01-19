#include "pch.h"
#include <glad/glad.h>

#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderer.h"

#include "Doge/Core/Window.h"
#include "Doge/Core/Application.h"

#include "Doge/Utility/Mesh.h"
#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"

#include "Doge/Renderer/RendererBindingTable.h"

namespace Doge
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
			case GL_DEBUG_SEVERITY_HIGH:         Log::Critical(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       Log::Error(message); return;
			case GL_DEBUG_SEVERITY_LOW:          Log::Warn(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: Log::Trace(message); return;
		}
	}

	struct DrawArraysIndirectCommand
	{
		GLuint Count;
		GLuint PrimCount;
		GLuint First;
		GLuint BaseInstance;

		DrawArraysIndirectCommand(GLuint count, GLuint primCount, GLuint first, GLuint baseInstance)
			: Count(count), PrimCount(primCount), First(first), BaseInstance(baseInstance)
		{

		}
	} static const indirectCmd(6, 2, 0, 0);

	struct GeometryBuffer
	{
		GLuint Framebuffer;
		GLuint PositionAttachment;
		GLuint NormalAttachment;
		GLuint AlbedoSpecularAttachment;
		GLuint DepthAttachment;

		uint32_t Width = 1280, Height = 720;
	};

	struct OpenGLRendererData
	{
		GLuint VertexArray;
		GeometryBuffer GBuffer;

		GLuint ViewProjectionUniformBuffer;
		GLuint TransformMatrixUniformBuffer;

		Ref<OpenGLShader> GeometryPassShader;
		Ref<OpenGLShader> LightingPassShader;

		Ref<OpenGLShader> SkyboxShader;
		Ref<OpenGLShader> TexturedQuadShader;

		const OpenGLShader* LastShaderState = nullptr;
	} static s_Data;

	OpenGLRenderer::OpenGLRenderer()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow());
		LOG_ASSERT(window, "Window could not found!");

		glfwMakeContextCurrent(window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LOG_ASSERT(status, "Glad initialization failed");

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height)
		{
			if (width == 0 || height == 0 || (s_Data.GBuffer.Width == width && s_Data.GBuffer.Height == height))
			{
				Log::Warn("Attempted to rezize framebuffer to {0}, {1}", width, height);
				return;
			}

			ConstructGBuffer(width, height);
		});

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

		OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
		s_Data.TexturedQuadShader = shaderCache->LoadShader(OpenGLRendererShaders::TexturedQuad);

		s_Data.GeometryPassShader = shaderCache->LoadShader(OpenGLRendererShaders::GeometryPass);

		s_Data.LightingPassShader = shaderCache->LoadShader(OpenGLRendererShaders::LightingPass);
		s_Data.LightingPassShader->Bind();
		s_Data.LightingPassShader->SetUniformInt("u_PositionAttachment", RendererBindingTable::GBufferPositionTextureUnit);
		s_Data.LightingPassShader->SetUniformInt("u_NormalAttachment", RendererBindingTable::GBufferNormalTextureUnit);
		s_Data.LightingPassShader->SetUniformInt("u_AlbedoSpecularAttachment", RendererBindingTable::GBufferAlbedoSpecularTextureUnit);
		
		BeginScene = [&](const Camera& camera)
		{
			// Update Uniform Buffers' contents
			glNamedBufferSubData(s_Data.ViewProjectionUniformBuffer, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
			glNamedBufferSubData(s_Data.ViewProjectionUniformBuffer, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetProjectionMatrix()));
			glNamedBufferSubData(s_Data.ViewProjectionUniformBuffer, 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.GetPosition()));
		};

		// Geometry Pass
		RenderPasses.push_back([]()
		{
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_FRAMEBUFFER, s_Data.GBuffer.Framebuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);

			s_Data.GeometryPassShader->Bind();

/*
// 			glDrawElementsBaseVertex(
// 				GL_TRIANGLES,
// 
// 			)
*/
		});

		// Lighting Pass
		RenderPasses.push_back([]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			s_Data.LightingPassShader->Bind();

			glDrawArraysIndirect(GL_TRIANGLES, &indirectCmd);
		});

		EndScene = [&]()
		{
			glBlitNamedFramebuffer(
				s_Data.GBuffer.Framebuffer,
				0, // Default Framebuffer
				0, 0,
				s_Data.GBuffer.Width,
				s_Data.GBuffer.Height,
				0, 0,
				s_Data.GBuffer.Width,
				s_Data.GBuffer.Height,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);

			// Render Framebuffer to the Screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			s_Data.LastShaderState = nullptr;

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			s_Data.TexturedQuadShader->Bind();

			glDrawArraysIndirect(GL_TRIANGLES, &indirectCmd);
		};

		SetSkybox = [&](CubemapTexture skybox)
		{
			if (!s_Data.SkyboxShader)
			{
				Shader skyboxShaderHandle = ShaderLibrary::LoadShader(OpenGLRendererShaders::Skybox);
				s_Data.SkyboxShader = (*OpenGLShaderCache::GetInstance())[skyboxShaderHandle];
			}

			glBindTextureUnit(RendererBindingTable::SkyboxTextureUnit, skybox);
			s_Data.SkyboxShader->SetUniformInt("u_Skybox", RendererBindingTable::SkyboxTextureUnit);
		};

		ClearSkybox = [&]()
		{
			glBindTextureUnit(RendererBindingTable::SkyboxTextureUnit, 0);
			s_Data.SkyboxShader.reset();
		};

		glCreateVertexArrays(1, &s_Data.VertexArray);
		glBindVertexArray(s_Data.VertexArray);

		const WindowProps& props = Application::GetActiveWindow()->GetWindowProps();
		ConstructGBuffer(props.Width, props.Height);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Log::Trace("Renderer initialized successfully!");
	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		glDeleteFramebuffers(1, &s_Data.GBuffer.Framebuffer);
		glDeleteTextures(1, &s_Data.GBuffer.PositionAttachment);
		glDeleteTextures(1, &s_Data.GBuffer.NormalAttachment);
		glDeleteTextures(1, &s_Data.GBuffer.AlbedoSpecularAttachment);
		glDeleteRenderbuffers(1, &s_Data.GBuffer.DepthAttachment);

		glDeleteVertexArrays(1, &s_Data.VertexArray);
	}

	void OpenGLRenderer::SetCommonUniformProperties(const Ref<Material>& material)
	{
		for (const auto& [name, prop] : material->GetProperties())
		{
			switch (static_cast<MaterialDataIndex>(prop.index()))
			{
				case MaterialDataIndex::Float:
				{
					float value = std::get<float>(prop);
					s_Data.LastShaderState->SetUniformFloat(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					s_Data.LastShaderState->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					s_Data.LastShaderState->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					s_Data.LastShaderState->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	void OpenGLRenderer::SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance)
	{
		for (const auto& [name, prop] : materialInstance->GetModifiedProperties())
		{
			switch (static_cast<MaterialDataIndex>(prop.index()))
			{
				case MaterialDataIndex::Float:
				{
					float value = std::get<float>(prop);
					s_Data.LastShaderState->SetUniformFloat(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					s_Data.LastShaderState->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					s_Data.LastShaderState->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					s_Data.LastShaderState->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	void OpenGLRenderer::ConstructGBuffer(uint32_t width, uint32_t height)
	{
		s_Data.GBuffer.Width = width;
		s_Data.GBuffer.Height = height;

		if (s_Data.GBuffer.Framebuffer)
		{
			glDeleteFramebuffers(1, &s_Data.GBuffer.Framebuffer);
			glDeleteTextures(1, &s_Data.GBuffer.PositionAttachment);
			glDeleteTextures(1, &s_Data.GBuffer.NormalAttachment);
			glDeleteTextures(1, &s_Data.GBuffer.AlbedoSpecularAttachment);
			glDeleteRenderbuffers(1, &s_Data.GBuffer.DepthAttachment);
		}

		glCreateFramebuffers(1, &s_Data.GBuffer.Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_Data.GBuffer.Framebuffer);

		// GBuffer Position Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.GBuffer.PositionAttachment);
		glBindTexture(GL_TEXTURE_2D, s_Data.GBuffer.PositionAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_Data.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT0, s_Data.GBuffer.PositionAttachment, 0);

		// GBuffer Normal Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.GBuffer.NormalAttachment);
		glBindTexture(GL_TEXTURE_2D, s_Data.GBuffer.NormalAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_Data.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT1,	s_Data.GBuffer.NormalAttachment, 0);

		// GBuffer Albedo - Specular Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.GBuffer.AlbedoSpecularAttachment);
		glBindTexture(GL_TEXTURE_2D, s_Data.GBuffer.AlbedoSpecularAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_Data.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT2,	s_Data.GBuffer.AlbedoSpecularAttachment, 0);

		static const GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(s_Data.GBuffer.Framebuffer, 3, attachments);

		glCreateRenderbuffers(1, &s_Data.GBuffer.DepthAttachment);
		glNamedRenderbufferStorage(s_Data.GBuffer.DepthAttachment, GL_DEPTH_COMPONENT, width, height);
		glNamedFramebufferRenderbuffer(s_Data.GBuffer.Framebuffer, GL_DEPTH_ATTACHMENT,	GL_RENDERBUFFER, s_Data.GBuffer.DepthAttachment);

		LOG_ASSERT(glCheckNamedFramebufferStatus(s_Data.GBuffer.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::GBufferPositionTextureUnit, s_Data.GBuffer.PositionAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferNormalTextureUnit, s_Data.GBuffer.NormalAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferAlbedoSpecularTextureUnit, s_Data.GBuffer.AlbedoSpecularAttachment);
		glViewport(0, 0, s_Data.GBuffer.Width, s_Data.GBuffer.Height);
	}
}