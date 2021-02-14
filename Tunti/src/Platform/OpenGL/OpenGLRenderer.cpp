#include "pch.h"
#include <glad/glad.h>

#include "OpenGLShader.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderer.h"
#include "OpenGLBufferManager.h"

#include "Tunti/Core/Window.h"
#include "Tunti/Core/Application.h"

#include "Tunti/Renderer/Mesh.h"
#include "Tunti/Renderer/Camera.h"
#include "Tunti/Renderer/Material.h"

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
#if DEBUG_ENABLED
				__debugbreak();
#endif
				return;
			}
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
	};

	struct DrawElementsIndirectCommand
	{
		GLuint Count;
		GLuint PrimCount;
		GLuint FirstIndex;
		GLuint BaseVertex;
		GLuint BaseInstance;

		DrawElementsIndirectCommand(GLuint count, GLuint primCount, GLuint firstIndex, GLuint baseVertex, GLuint baseInstance)
			: Count(count), PrimCount(primCount), FirstIndex(firstIndex), BaseVertex(baseVertex), BaseInstance(baseInstance)
		{

		}
	};

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
		GLuint ViewProjectionUniformBuffer;
		GLuint LightsUniformBuffer;

		Ref<OpenGLShader> SkyboxShader;
		Ref<OpenGLShader> TexturedQuadShader;

		const DrawArraysIndirectCommand QuadIndirectCommand = DrawArraysIndirectCommand(3, 1, 0, 0);
	} static s_Data;

	struct OpenGLDeferredRendererData
	{
		GeometryBuffer GBuffer;

		Ref<OpenGLShader> GeometryPassShader;
		Ref<OpenGLShader> PBRDeferredPassShader;
	} static s_DeferredData;

	OpenGLRenderer::OpenGLRenderer()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::GetActiveWindow()->GetNativeWindow());
		LOG_ASSERT(window, "Window could not found!");

		glfwMakeContextCurrent(window);
		glfwSwapInterval(static_cast<int>(Application::GetActiveWindow()->GetWindowProps().VSync));

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

		glCreateVertexArrays(1, &s_Data.VertexArray);
		glBindVertexArray(s_Data.VertexArray);

		glCreateBuffers(1, &s_Data.LightsUniformBuffer);
		glNamedBufferStorage(s_Data.LightsUniformBuffer, sizeof(LightQueueContainer), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::LightsUniformBuffer, s_Data.LightsUniformBuffer);

		glCreateBuffers(1, &s_Data.ViewProjectionUniformBuffer);
		glNamedBufferStorage(s_Data.ViewProjectionUniformBuffer, 2 * sizeof(glm::mat4) + sizeof(glm::vec3), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::ViewProjectionUniformBuffer, s_Data.ViewProjectionUniformBuffer);

		OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
		s_Data.TexturedQuadShader = shaderCache->LoadShader(RendererShaders::TexturedQuad);

		InitDeferredRenderer(window);

		BeginScene = [&](const Camera& camera, const glm::mat4& transform, const glm::vec3& position)
		{
			// Update Uniform Buffers' contents
			glNamedBufferSubData(s_Data.ViewProjectionUniformBuffer, 0, sizeof(glm::mat4), glm::value_ptr(camera.GetProjection() * glm::inverse(transform)));
			glNamedBufferSubData(s_Data.ViewProjectionUniformBuffer, sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(position));
		};

		EndScene = [&]()
		{			
			
		};

		SetSkybox = [&](CubemapTexture skybox)
		{
			if (!s_Data.SkyboxShader)
			{
				OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
				s_Data.SkyboxShader = shaderCache->LoadShader(RendererShaders::Skybox);
			}

			glBindTextureUnit(RendererBindingTable::SkyboxTextureUnit, skybox);
			s_Data.SkyboxShader->SetUniformInt("u_Skybox", RendererBindingTable::SkyboxTextureUnit);
		};

		ClearSkybox = [&]()
		{
			glBindTextureUnit(RendererBindingTable::SkyboxTextureUnit, 0);
			s_Data.SkyboxShader.reset();
		};		
	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		DestroyDeferredRenderer();

		glDeleteVertexArrays(1, &s_Data.VertexArray);
		glDeleteBuffers(1, &s_Data.LightsUniformBuffer);
		glDeleteBuffers(1, &s_Data.ViewProjectionUniformBuffer);
	}

	void OpenGLRenderer::SetCommonUniformProperties(const Ref<Material>& material, const Ref<OpenGLShader>& shader)
	{
		for (const auto& [name, prop] : material->GetProperties())
		{
			switch (static_cast<MaterialDataIndex>(prop.index()))
			{
				case MaterialDataIndex::Float:
				{
					float value = std::get<float>(prop);
					shader->SetUniformFloat(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					shader->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					shader->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					shader->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	void OpenGLRenderer::SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance, const Ref<OpenGLShader>& shader)
	{
		for (const auto& [name, prop] : materialInstance->GetModifiedProperties())
		{
			switch (static_cast<MaterialDataIndex>(prop.index()))
			{
				case MaterialDataIndex::Float:
				{
					float value = std::get<float>(prop);
					shader->SetUniformFloat(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float2:
				{
					const glm::vec2& value = std::get<glm::vec2>(prop);
					shader->SetUniformFloat2(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float3:
				{
					const glm::vec3& value = std::get<glm::vec3>(prop);
					shader->SetUniformFloat3(name.c_str(), value);
					break;
				}
				case MaterialDataIndex::Float4:
				{
					const glm::vec4& value = std::get<glm::vec4>(prop);
					shader->SetUniformFloat4(name.c_str(), value);
					break;
				}
			}
		}
	}

	/************************************************************************/
	/*					Deferred Render Pipeline                            */
	/************************************************************************/

	void OpenGLRenderer::InitDeferredRenderer(GLFWwindow* window)
	{
		const WindowProps& props = Application::GetActiveWindow()->GetWindowProps();
		ConstructGBuffer(props.Width, props.Height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height)
		{
			if (width == 0 || height == 0 || (s_DeferredData.GBuffer.Width == width && s_DeferredData.GBuffer.Height == height))
			{
				Log::Warn("Attempted to rezize framebuffer to {0}, {1}", width, height);
				return;
			}

			ConstructGBuffer(width, height);
		});

		OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
		s_DeferredData.GeometryPassShader = shaderCache->LoadShader(RendererShaders::GeometryPass);

		s_DeferredData.PBRDeferredPassShader = shaderCache->LoadShader(RendererShaders::PBRDeferredPass);
		s_DeferredData.PBRDeferredPassShader->SetUniformInt("u_PositionAttachment", RendererBindingTable::GBufferPositionTextureUnit);
		s_DeferredData.PBRDeferredPassShader->SetUniformInt("u_NormalAttachment", RendererBindingTable::GBufferNormalTextureUnit);
		s_DeferredData.PBRDeferredPassShader->SetUniformInt("u_AlbedoSpecularAttachment", RendererBindingTable::GBufferAlbedoSpecularTextureUnit);

		// Geometry Pass
		RenderPasses.push_back([&]()
		{
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData.GBuffer.Framebuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			s_DeferredData.GeometryPassShader->Bind();
			for (const auto& [material, materialInstanceMap] : MeshQueue)
			{
				SetCommonUniformProperties(material, s_DeferredData.GeometryPassShader);
				for (const auto& [materialInstance, meshArray] : materialInstanceMap)
				{
					SetUniqueUniformProperties(materialInstance, s_DeferredData.GeometryPassShader);
					const OpenGLGraphicsBuffer& buffer = (*OpenGLBufferManager::GetInstance())[std::hash<Ref<MaterialInstance>>{}(materialInstance)];
					GLuint buffers[2] = { buffer.TextureMapIndexBuffer, buffer.VertexBuffer };
					glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, RendererBindingTable::TextureMapIndexShaderStorageBuffer, 2, buffers);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.IndexBuffer);

					s_DeferredData.GeometryPassShader->SetUniformUInt("u_VertexCount", buffer.VertexCount);
					for (const auto& [mesh, transform] : meshArray)
					{
						s_DeferredData.GeometryPassShader->SetUniformMat4("u_Model", transform);
						DrawElementsIndirectCommand indirectCmd(mesh.Count, 1, mesh.BaseIndex, mesh.BaseVertex, 0);
						glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &indirectCmd);
					}
				}
			}
		});

		// Lighting Pass
		RenderPasses.push_back([&]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Update Light Buffer's content
			glNamedBufferSubData(s_Data.LightsUniformBuffer, 0, sizeof(glm::vec4) + sizeof(LightData) * LightQueue.LightCount, &LightQueue);

			s_DeferredData.PBRDeferredPassShader->Bind();
			glDrawArraysIndirect(GL_TRIANGLES, &s_Data.QuadIndirectCommand);
		});
	}

	void OpenGLRenderer::DestroyDeferredRenderer()
	{
		glDeleteFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);
		glDeleteTextures(1, &s_DeferredData.GBuffer.PositionAttachment);
		glDeleteTextures(1, &s_DeferredData.GBuffer.NormalAttachment);
		glDeleteTextures(1, &s_DeferredData.GBuffer.AlbedoSpecularAttachment);
		glDeleteRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);
	}

	void OpenGLRenderer::ConstructGBuffer(uint32_t width, uint32_t height)
	{
		s_DeferredData.GBuffer.Width = width;
		s_DeferredData.GBuffer.Height = height;

		if (s_DeferredData.GBuffer.Framebuffer)
		{
			glDeleteFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);
			glDeleteTextures(1, &s_DeferredData.GBuffer.PositionAttachment);
			glDeleteTextures(1, &s_DeferredData.GBuffer.NormalAttachment);
			glDeleteTextures(1, &s_DeferredData.GBuffer.AlbedoSpecularAttachment);
			glDeleteRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);
		}

		glCreateFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData.GBuffer.Framebuffer);

		// GBuffer Position Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.PositionAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.PositionAttachment, 1, GL_RGBA16F, width, height);
		glTextureParameteri(s_DeferredData.GBuffer.PositionAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.PositionAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT0, s_DeferredData.GBuffer.PositionAttachment, 0);

		// GBuffer Normal Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.NormalAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.NormalAttachment, 1, GL_RGBA16F, width, height);
		glTextureParameteri(s_DeferredData.GBuffer.NormalAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.NormalAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT1, s_DeferredData.GBuffer.NormalAttachment, 0);

		// GBuffer Albedo - Specular Pass
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.AlbedoSpecularAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.AlbedoSpecularAttachment, 1, GL_RGBA8, width, height);
		glTextureParameteri(s_DeferredData.GBuffer.AlbedoSpecularAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.AlbedoSpecularAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT2, s_DeferredData.GBuffer.AlbedoSpecularAttachment, 0);

		static const GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(s_DeferredData.GBuffer.Framebuffer, 3, attachments);

		glCreateRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);
		glNamedRenderbufferStorage(s_DeferredData.GBuffer.DepthAttachment, GL_DEPTH_COMPONENT, width, height);
		glNamedFramebufferRenderbuffer(s_DeferredData.GBuffer.Framebuffer, GL_DEPTH_ATTACHMENT,	GL_RENDERBUFFER, s_DeferredData.GBuffer.DepthAttachment);

		LOG_ASSERT(glCheckNamedFramebufferStatus(s_DeferredData.GBuffer.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::GBufferPositionTextureUnit, s_DeferredData.GBuffer.PositionAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferNormalTextureUnit, s_DeferredData.GBuffer.NormalAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferAlbedoSpecularTextureUnit, s_DeferredData.GBuffer.AlbedoSpecularAttachment);
		glViewport(0, 0, s_DeferredData.GBuffer.Width, s_DeferredData.GBuffer.Height);
	}
}