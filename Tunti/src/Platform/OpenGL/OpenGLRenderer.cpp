#include "pch.h"
#include <glad/glad.h>

#include "OpenGLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderer.h"
#include "OpenGLMaterial.h"

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
				LOG_ASSERT(false);
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
		GLuint NormalAttachment;
		GLuint AlbedoAttachment;
		GLuint RoughnessMetalnessAOAttachment;
		GLuint DepthAttachment;
	};

	struct ShadowPassData
	{
		GLuint Framebuffer;
		GLuint ShadowMap;

		uint32_t Resolution = 1024;
	};

	struct OpenGLRendererData
	{
		GLuint VertexArray;
		GLuint CameraUniformBuffer;
		GLuint LightsUniformBuffer;

		GLuint ScreenFramebuffer;
		GLuint ScreenFramebufferColorAttachment;
		uint32_t ScreenWidth = 1280, ScreenHeight = 720;

		const DrawArraysIndirectCommand QuadIndirectCommand = DrawArraysIndirectCommand(3, 1, 0, 0);
		const DrawArraysIndirectCommand CubeIndirectCommand = DrawArraysIndirectCommand(14, 1, 0, 0);
	} static s_Data;

	struct OpenGLDeferredRendererData
	{
		GeometryBuffer GBuffer;
		ShadowPassData ShadowPass;

		Ref<OpenGLShaderProgram> ShadowPassShader;
		Ref<OpenGLShaderProgram> GeometryPassShader;
		Ref<OpenGLShaderProgram> PBRLightingPassShader;
		Ref<OpenGLShaderProgram> SkyboxPassShader;
	} static s_DeferredData;

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

		glCreateVertexArrays(1, &s_Data.VertexArray);
		glBindVertexArray(s_Data.VertexArray);

		glCreateBuffers(1, &s_Data.LightsUniformBuffer);
		glNamedBufferStorage(s_Data.LightsUniformBuffer, sizeof(LightQueueContainer), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::LightsUniformBuffer, s_Data.LightsUniformBuffer);

		glCreateBuffers(1, &s_Data.CameraUniformBuffer);
		glNamedBufferStorage(s_Data.CameraUniformBuffer, sizeof(CameraBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::CameraUniformBuffer, s_Data.CameraUniformBuffer);

		InitDeferredRenderer(Application::GetWindow()->GetHandle());

		BeginScene = [&](const Camera& camera, const glm::mat4& view, const glm::vec3& position)
		{
			// Update Uniform Buffers' contents
			CameraBuffer cameraBuffer{ view, camera.GetProjection(), position };
			glNamedBufferSubData(s_Data.CameraUniformBuffer, 0, sizeof(CameraBuffer), &cameraBuffer);

			// Update Light Buffer's content
			glNamedBufferSubData(s_Data.LightsUniformBuffer, 0, sizeof(LightQueueContainer), &LightQueue);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		};

		EndScene = [&]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBlitNamedFramebuffer(s_Data.ScreenFramebuffer, 0,
				0, 0, s_Data.ScreenWidth, s_Data.ScreenHeight,
				0, 0, s_Data.ScreenWidth, s_Data.ScreenHeight,
				GL_COLOR_BUFFER_BIT, GL_NEAREST);
		};

		ResizeFramebuffers = [&](uint32_t width, uint32_t height)
		{
			if (width == 0 || height == 0 || (s_Data.ScreenWidth == width && s_Data.ScreenHeight == height))
			{
				return;
			}

			ConstructScreenBuffer(width, height);
			ConstructGBuffer();
		};

		GetFramebufferTexture = [&]()
		{
			return Texture2D(s_Data.ScreenFramebufferColorAttachment);
		};

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		SetEnvironmentMap(OpenGLTextureCache::GetInstance()->CreateEnvironmentMap(RendererConstants::DefaultEnvironmentMap));
	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		DestroyDeferredRenderer();

		glDeleteVertexArrays(1, &s_Data.VertexArray);
		glDeleteBuffers(1, &s_Data.LightsUniformBuffer);
		glDeleteBuffers(1, &s_Data.CameraUniformBuffer);

		glDeleteFramebuffers(1, &s_Data.ScreenFramebuffer);
		glDeleteTextures(1, &s_Data.ScreenFramebufferColorAttachment);
	}

	/************************************************************************/
	/*					Deferred Render Pipeline                            */
	/************************************************************************/

	void OpenGLRenderer::InitDeferredRenderer(GLFWwindow* window)
	{
		const WindowProps& props = Application::GetWindow()->GetWindowProps();
		ConstructScreenBuffer(props.Width, props.Height);
		ConstructGBuffer();
		ConstructShadowMapBuffer(1024);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height)
		{
			if (width == 0 || height == 0 || (s_Data.ScreenWidth == width && s_Data.ScreenHeight == height))
			{
				Log::Warn("Attempted to rezize framebuffer to {0}, {1}", width, height);
				return;
			}

			ConstructScreenBuffer(width, height);
			ConstructGBuffer();
		});

		SetEnvironmentMap = [&](EnvironmentMapTexture environmentMap)
		{
			if (!s_DeferredData.SkyboxPassShader)
			{
				OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
				s_DeferredData.SkyboxPassShader = shaderCache->LoadShaderProgram(RendererShaders::SkyboxPass);
			}

			glBindTextureUnit(RendererBindingTable::PrefilterSpecularCubemapTextureUnit, environmentMap.PrefilteredSpecularMapTextureID);
			glBindTextureUnit(RendererBindingTable::IrradianceCubemapTextureUnit, environmentMap.IrradianceMapTextureID);
			glBindTextureUnit(RendererBindingTable::BRDF_LUTCubemapTextureUnit, environmentMap.BRDF_LUTTextureID);
		};

		OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
		s_DeferredData.ShadowPassShader = shaderCache->LoadShaderProgram(RendererShaders::ShadowPass);
		s_DeferredData.GeometryPassShader = shaderCache->LoadShaderProgram(RendererShaders::GeometryPass);

		s_DeferredData.PBRLightingPassShader = shaderCache->LoadShaderProgram(RendererShaders::PBRLightingPass);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_DepthAttachment", RendererBindingTable::GBufferDepthTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_NormalAttachment", RendererBindingTable::GBufferNormalTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_AlbedoAttachment", RendererBindingTable::GBufferAlbedoTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_RoughnessMetalnessAOAttachment", RendererBindingTable::GBufferRoughnessMetalnessAOTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_SpecularCubemap", RendererBindingTable::PrefilterSpecularCubemapTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_IrradianceCubemap", RendererBindingTable::IrradianceCubemapTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_SpecularBRDF_LUT", RendererBindingTable::BRDF_LUTCubemapTextureUnit);
		s_DeferredData.PBRLightingPassShader->SetUniformInt("u_DirectionalLightShadowMap", RendererBindingTable::DirectionalLightShadowMapTextureUnit);

		s_DeferredData.SkyboxPassShader = shaderCache->LoadShaderProgram(RendererShaders::SkyboxPass);
		s_DeferredData.SkyboxPassShader->SetUniformInt("u_Skybox", RendererBindingTable::PrefilterSpecularCubemapTextureUnit);

		// Shadow Pass
		RenderPasses.push_back([&]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData.ShadowPass.Framebuffer);
			glViewport(0, 0, s_DeferredData.ShadowPass.Resolution, s_DeferredData.ShadowPass.Resolution);
			glClear(GL_DEPTH_BUFFER_BIT);

			s_DeferredData.ShadowPassShader->Bind();

			// Directional Light Shadow Mapping
			{
				for (const auto& [material, meshQueue] : MeshQueue)
				{
					for (const auto& [mesh, submeshQueue] : meshQueue)
					{
						OpenGLBufferCache::GetInstance()->GetOpenGLMeshBufferWithKey(mesh).Bind();
						for (auto& [submeshQueueElementList, transform] : submeshQueue)
						{
							s_DeferredData.ShadowPassShader->SetUniformMat4("u_Model", transform);
							const auto& [submeshes, _] = submeshQueueElementList;
							for (const auto& submesh : submeshes)
							{
								DrawElementsIndirectCommand indirectCmd(submesh.Count, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
								glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &indirectCmd);
							}
						}
					}
				}
			}
			
			// Point Lights Shadow Mapping
			for (uint32_t i = 0; i < LightQueue.PointLightCount; i++)
			{
				// TODO:
			}
		});

		// Geometry Pass
		RenderPasses.push_back([&]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, s_DeferredData.GBuffer.Framebuffer);
			glViewport(0, 0, s_Data.ScreenWidth, s_Data.ScreenHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			s_DeferredData.GeometryPassShader->Bind();
			for (const auto& [material, meshQueue] : MeshQueue)
			{
				(*OpenGLMaterialCache::GetInstance())[material->Index].Bind();
				for (const auto& [mesh, submeshQueue] : meshQueue)
				{
					const OpenGLMeshBuffer& meshBuffer = OpenGLBufferCache::GetInstance()->GetOpenGLMeshBufferWithKey(mesh);
					meshBuffer.Bind();
					s_DeferredData.GeometryPassShader->SetUniformUInt("u_VertexCount", meshBuffer.GetVertexCount());
					for (const auto& [submeshQueueElementList, transform] : submeshQueue)
					{
						s_DeferredData.GeometryPassShader->SetUniformMat4("u_Model", transform);
						const auto& [submeshes, materialInstances] = submeshQueueElementList;

						for (const auto& submesh : submeshes)
						{
							DrawElementsIndirectCommand indirectCmd(submesh.Count, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
							glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &indirectCmd);
						}
					}
				}
			}
		});

		// Lighting Pass
		RenderPasses.push_back([&]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, s_Data.ScreenFramebuffer);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);

			s_DeferredData.PBRLightingPassShader->Bind();
			glDrawArraysIndirect(GL_TRIANGLE_STRIP, &s_Data.QuadIndirectCommand);
		});

		// Skybox Pass
		RenderPasses.push_back([&]()
		{
			glDepthRange(1.0f, 1.0f);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);

			s_DeferredData.SkyboxPassShader->Bind();
			glDrawArraysIndirect(GL_TRIANGLE_STRIP, &s_Data.CubeIndirectCommand);

			glDepthRange(0.0f, 1.0f);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);

			glCullFace(GL_BACK);
		});
	}

	void OpenGLRenderer::DestroyDeferredRenderer()
	{
		glDeleteFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);
		glDeleteTextures(1, &s_DeferredData.GBuffer.NormalAttachment);
		glDeleteTextures(1, &s_DeferredData.GBuffer.AlbedoAttachment);
		glDeleteRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);

		glDeleteFramebuffers(1, &s_DeferredData.ShadowPass.Framebuffer);
		glDeleteRenderbuffers(1, &s_DeferredData.ShadowPass.ShadowMap);
	}

	void OpenGLRenderer::ConstructScreenBuffer(uint32_t width, uint32_t height)
	{
		s_Data.ScreenWidth = width;
		s_Data.ScreenHeight = height;

		glCreateFramebuffers(1, &s_Data.ScreenFramebuffer);

		glCreateTextures(GL_TEXTURE_2D, 1, &s_Data.ScreenFramebufferColorAttachment);
		glTextureStorage2D(s_Data.ScreenFramebufferColorAttachment, 1, GL_RGB32F, s_Data.ScreenWidth, s_Data.ScreenHeight);
		glTextureParameteri(s_Data.ScreenFramebufferColorAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_Data.ScreenFramebufferColorAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_Data.ScreenFramebuffer, GL_COLOR_ATTACHMENT0, s_Data.ScreenFramebufferColorAttachment, 0);

		LOG_ASSERT(glCheckNamedFramebufferStatus(s_Data.ScreenFramebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");
	}

	void OpenGLRenderer::ConstructGBuffer()
	{
		if (s_DeferredData.GBuffer.Framebuffer)
		{
			glDeleteFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);
			glDeleteTextures(1, &s_DeferredData.GBuffer.NormalAttachment);
			glDeleteTextures(1, &s_DeferredData.GBuffer.AlbedoAttachment);
			glDeleteRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);
		}
		glCreateFramebuffers(1, &s_DeferredData.GBuffer.Framebuffer);

		// GBuffer Normal Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.NormalAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.NormalAttachment, 1, GL_RGB16F, s_Data.ScreenWidth, s_Data.ScreenHeight);
		glTextureParameteri(s_DeferredData.GBuffer.NormalAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.NormalAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT0, s_DeferredData.GBuffer.NormalAttachment, 0);

		// GBuffer Albedo Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.AlbedoAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.AlbedoAttachment, 1, GL_RGB8, s_Data.ScreenWidth, s_Data.ScreenHeight);
		glTextureParameteri(s_DeferredData.GBuffer.AlbedoAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.AlbedoAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT1, s_DeferredData.GBuffer.AlbedoAttachment, 0);

		// GBuffer Material Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment);
		glTextureStorage2D(s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment, 1, GL_RGB8, s_Data.ScreenWidth, s_Data.ScreenHeight);
		glTextureParameteri(s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(s_DeferredData.GBuffer.Framebuffer, GL_COLOR_ATTACHMENT2, s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment, 0);

		const GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(s_DeferredData.GBuffer.Framebuffer, 3, attachments);

		// GBuffer Position Attachment
		glCreateRenderbuffers(1, &s_DeferredData.GBuffer.DepthAttachment);
		glNamedRenderbufferStorage(s_DeferredData.GBuffer.DepthAttachment, GL_DEPTH_COMPONENT32F, s_Data.ScreenWidth, s_Data.ScreenHeight);
		glNamedFramebufferRenderbuffer(s_DeferredData.GBuffer.Framebuffer, GL_DEPTH_ATTACHMENT,	GL_RENDERBUFFER, s_DeferredData.GBuffer.DepthAttachment);

		LOG_ASSERT(glCheckNamedFramebufferStatus(s_DeferredData.GBuffer.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::GBufferDepthTextureUnit, s_DeferredData.GBuffer.DepthAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferNormalTextureUnit, s_DeferredData.GBuffer.NormalAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferAlbedoTextureUnit, s_DeferredData.GBuffer.AlbedoAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferRoughnessMetalnessAOTextureUnit, s_DeferredData.GBuffer.RoughnessMetalnessAOAttachment);
		glViewport(0, 0, s_Data.ScreenWidth, s_Data.ScreenHeight);

		glNamedFramebufferRenderbuffer(s_Data.ScreenFramebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_DeferredData.GBuffer.DepthAttachment);
	}

	void OpenGLRenderer::ConstructShadowMapBuffer(uint32_t resolution)
	{
		s_DeferredData.ShadowPass.Resolution = resolution;
		glCreateFramebuffers(1, &s_DeferredData.ShadowPass.Framebuffer);

		glCreateTextures(GL_TEXTURE_2D, 1, &s_DeferredData.ShadowPass.ShadowMap);
		glTextureStorage2D(s_DeferredData.ShadowPass.ShadowMap, 1, GL_DEPTH_COMPONENT32F, s_DeferredData.ShadowPass.Resolution, s_DeferredData.ShadowPass.Resolution);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(s_DeferredData.ShadowPass.ShadowMap, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glNamedFramebufferTexture(s_DeferredData.ShadowPass.Framebuffer, GL_DEPTH_ATTACHMENT, s_DeferredData.ShadowPass.ShadowMap, 0);

		glNamedFramebufferDrawBuffer(s_DeferredData.ShadowPass.Framebuffer, GL_NONE);
		glNamedFramebufferReadBuffer(s_DeferredData.ShadowPass.Framebuffer, GL_NONE);

		LOG_ASSERT(glCheckNamedFramebufferStatus(s_DeferredData.ShadowPass.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::DirectionalLightShadowMapTextureUnit, s_DeferredData.ShadowPass.ShadowMap);
	}
}