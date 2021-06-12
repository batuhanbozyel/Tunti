#include "pch.h"
#include <glad/glad.h>

#include "OpenGLDeferredRenderPipeline.h"
#include "OpenGLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLMaterial.h"

#include "Tunti/Core/Window.h"
#include "Tunti/Core/Application.h"
#include "Tunti/Scene/SceneSettings.h"

namespace Tunti
{
	OpenGLDeferredRenderPipeline::OpenGLDeferredRenderPipeline()
	{
		glCreateBuffers(1, &m_LightsUniformBuffer);
		glNamedBufferStorage(m_LightsUniformBuffer, sizeof(LightQueueContainer), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::LightsUniformBuffer, m_LightsUniformBuffer);

		glCreateBuffers(1, &m_CameraUniformBuffer);
		glNamedBufferStorage(m_CameraUniformBuffer, sizeof(ShaderCameraContainer), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, RendererBindingTable::CameraUniformBuffer, m_CameraUniformBuffer);

		const WindowProps& props = Application::GetWindow()->GetWindowProps();
		m_OutputWidth = props.Width;
		m_OutputHeight = props.Height;

		ConstructOutputBuffer();
		ConstructGBuffer();
		ConstructShadowMapBuffer(SceneSettings::ShadowMap::Resolution);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		OpenGLShaderCache* shaderCache = OpenGLShaderCache::GetInstance();
		m_ShadowPassShader = shaderCache->LoadShaderProgram(RendererShaders::ShadowPass);
		m_GeometryPassShader = shaderCache->LoadShaderProgram(RendererShaders::GeometryPass);

		m_PBRLightingPassShader = shaderCache->LoadShaderProgram(RendererShaders::PBRLightingPass);
		m_PBRLightingPassShader->SetUniformInt("u_DepthAttachment", RendererBindingTable::GBufferDepthTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_NormalAttachment", RendererBindingTable::GBufferNormalTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_AlbedoAttachment", RendererBindingTable::GBufferAlbedoTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_LightSpacePositionAttachment", RendererBindingTable::GBufferLightSpacePositionTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_SpecularCubemap", RendererBindingTable::PrefilterSpecularCubemapTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_IrradianceCubemap", RendererBindingTable::IrradianceCubemapTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_SpecularBRDF_LUT", RendererBindingTable::BRDF_LUTCubemapTextureUnit);
		m_PBRLightingPassShader->SetUniformInt("u_DirectionalLightShadowMap", RendererBindingTable::DirectionalLightShadowMapTextureUnit);

		m_SkyboxPassShader = shaderCache->LoadShaderProgram(RendererShaders::SkyboxPass);
		m_SkyboxPassShader->SetUniformInt("u_Skybox", RendererBindingTable::PrefilterSpecularCubemapTextureUnit);
	}

	OpenGLDeferredRenderPipeline::~OpenGLDeferredRenderPipeline()
	{
		glDeleteFramebuffers(1, &m_GBuffer.Framebuffer);
		glDeleteTextures(1, &m_GBuffer.NormalAttachment);
		glDeleteTextures(1, &m_GBuffer.AlbedoAttachment);
		glDeleteRenderbuffers(1, &m_GBuffer.DepthAttachment);

		glDeleteFramebuffers(1, &m_ShadowPass.Framebuffer);
		glDeleteRenderbuffers(1, &m_ShadowPass.ShadowMap);

		glDeleteBuffers(1, &m_LightsUniformBuffer);
		glDeleteBuffers(1, &m_CameraUniformBuffer);

		glDeleteFramebuffers(1, &m_OutputBuffer.Framebuffer);
		glDeleteTextures(1, &m_OutputBuffer.ColorAttachment);
	}

	Texture2D OpenGLDeferredRenderPipeline::ExecuteImpl(const ShaderCameraContainer& shaderCamera) const
	{
		UpdateBuffers(shaderCamera);
		ShadowPass();
		GeometryPass();
		LightingPass();
		SkyboxPass();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return Texture2D(m_OutputBuffer.ColorAttachment);
	}

	Texture2D OpenGLDeferredRenderPipeline::GetDebugOutputTexture(uint32_t idx) const
	{
		return Texture2D(m_ShadowPass.ShadowMap);
	}

	void OpenGLDeferredRenderPipeline::OnWindowResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || (m_OutputWidth == width && m_OutputHeight == height))
		{
			Log::Warn("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_OutputWidth = width;
		m_OutputHeight = height;

		ConstructOutputBuffer();
		ConstructGBuffer();
	}

	void OpenGLDeferredRenderPipeline::UpdateBuffers(const ShaderCameraContainer& shaderCamera) const
	{
		// Update Uniform Buffers' contents
		glNamedBufferSubData(m_CameraUniformBuffer, 0, sizeof(ShaderCameraContainer), &shaderCamera);

		// Update Light Buffer's content
		glNamedBufferSubData(m_LightsUniformBuffer, 0, sizeof(LightQueueContainer), &LightQueue);

		glBindTextureUnit(RendererBindingTable::PrefilterSpecularCubemapTextureUnit, EnvironmentMap.PrefilteredSpecularMapTextureID);
		glBindTextureUnit(RendererBindingTable::IrradianceCubemapTextureUnit, EnvironmentMap.IrradianceMapTextureID);
		glBindTextureUnit(RendererBindingTable::BRDF_LUTCubemapTextureUnit, EnvironmentMap.BRDF_LUTTextureID);
	}

	void OpenGLDeferredRenderPipeline::ShadowPass() const
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glDepthMask(GL_TRUE);

		glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowPass.Framebuffer);
		glViewport(0, 0, m_ShadowPass.Resolution, m_ShadowPass.Resolution);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Directional Light Shadow Mapping
		DrawSceneGeometry(m_ShadowPassShader);

		// Point Lights Shadow Mapping
		for (uint32_t i = 0; i < LightQueue.PointLightCount; i++)
		{
			// TODO:
		}
	}

	void OpenGLDeferredRenderPipeline::GeometryPass() const
	{
		glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.Framebuffer);
		glViewport(0, 0, m_OutputWidth, m_OutputHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawSceneGeometryWithMaterials(m_GeometryPassShader);
	}

	void OpenGLDeferredRenderPipeline::LightingPass() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_OutputBuffer.Framebuffer);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);

		m_PBRLightingPassShader->Bind();
		m_PBRLightingPassShader->SetUniformFloat("u_EnvironmentMapIntensity", SceneSettings::Lighting::EnvironmentMapIntensity);
		glDrawArraysIndirect(GL_TRIANGLE_STRIP, &QuadIndirectParams);
	}

	void OpenGLDeferredRenderPipeline::SkyboxPass() const
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glDepthRange(1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);

		m_SkyboxPassShader->Bind();
		glDrawArraysIndirect(GL_TRIANGLE_STRIP, &CubeIndirectParams);

		glDepthRange(0.0f, 1.0f);
		glDepthFunc(GL_LESS);
	}

	void OpenGLDeferredRenderPipeline::DrawSceneGeometry(const Ref<OpenGLShaderProgram>& shader) const
	{
		shader->Bind();
		for (const auto& [material, meshQueue] : MeshQueue)
		{
			for (const auto& [mesh, submeshQueue] : meshQueue)
			{
				OpenGLBufferCache::GetInstance()->GetOpenGLMeshBufferWithKey(mesh).Bind();
				for (auto& [submeshQueueElementList, transform] : submeshQueue)
				{
					shader->SetUniformMat4("u_Model", transform);
					const auto& [submeshes, _] = submeshQueueElementList;
					for (const auto& submesh : submeshes)
					{
						DrawElementsIndirectParams indirectParams(submesh.Count, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
						glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &indirectParams);
					}
				}
			}
		}
	}

	void OpenGLDeferredRenderPipeline::DrawSceneGeometryWithMaterials(const Ref<OpenGLShaderProgram>& shader) const
	{
		shader->Bind();
		for (const auto& [material, meshQueue] : MeshQueue)
		{
			(*OpenGLMaterialCache::GetInstance())[material->Index].Bind();
			for (const auto& [mesh, submeshQueue] : meshQueue)
			{
				const OpenGLMeshBuffer& meshBuffer = OpenGLBufferCache::GetInstance()->GetOpenGLMeshBufferWithKey(mesh);
				meshBuffer.Bind();
				shader->SetUniformUInt("u_VertexCount", meshBuffer.GetVertexCount());
				for (const auto& [submeshQueueElementList, transform] : submeshQueue)
				{
					shader->SetUniformMat4("u_Model", transform);
					const auto& [submeshes, materialInstances] = submeshQueueElementList;

					for (const auto& submesh : submeshes)
					{
						DrawElementsIndirectParams indirectParams(submesh.Count, 1, submesh.BaseIndex, submesh.BaseVertex, 0);
						glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &indirectParams);
					}
				}
			}
		}
	}

	void OpenGLDeferredRenderPipeline::ConstructOutputBuffer()
	{
		if (m_OutputBuffer.Framebuffer)
		{
			glDeleteFramebuffers(1, &m_OutputBuffer.Framebuffer);
			glDeleteTextures(1, &m_OutputBuffer.ColorAttachment);
		}

		glCreateFramebuffers(1, &m_OutputBuffer.Framebuffer);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_OutputBuffer.ColorAttachment);
		glTextureStorage2D(m_OutputBuffer.ColorAttachment, 1, GL_RGB32F, m_OutputWidth, m_OutputHeight);
		glTextureParameteri(m_OutputBuffer.ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_OutputBuffer.ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(m_OutputBuffer.Framebuffer, GL_COLOR_ATTACHMENT0, m_OutputBuffer.ColorAttachment, 0);

		LOG_ASSERT(glCheckNamedFramebufferStatus(m_OutputBuffer.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");
	}

	void OpenGLDeferredRenderPipeline::ConstructGBuffer()
	{
		if (m_GBuffer.Framebuffer)
		{
			glDeleteFramebuffers(1, &m_GBuffer.Framebuffer);
			glDeleteTextures(1, &m_GBuffer.NormalAttachment);
			glDeleteTextures(1, &m_GBuffer.AlbedoAttachment);
			glDeleteRenderbuffers(1, &m_GBuffer.DepthAttachment);
		}
		glCreateFramebuffers(1, &m_GBuffer.Framebuffer);

		// GBuffer Normal Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_GBuffer.NormalAttachment);
		glTextureStorage2D(m_GBuffer.NormalAttachment, 1, GL_RGBA16F, m_OutputWidth, m_OutputHeight);
		glTextureParameteri(m_GBuffer.NormalAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_GBuffer.NormalAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(m_GBuffer.Framebuffer, GL_COLOR_ATTACHMENT0, m_GBuffer.NormalAttachment, 0);

		// GBuffer Albedo Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_GBuffer.AlbedoAttachment);
		glTextureStorage2D(m_GBuffer.AlbedoAttachment, 1, GL_RGBA8, m_OutputWidth, m_OutputHeight);
		glTextureParameteri(m_GBuffer.AlbedoAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_GBuffer.AlbedoAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(m_GBuffer.Framebuffer, GL_COLOR_ATTACHMENT1, m_GBuffer.AlbedoAttachment, 0);

		// GBuffer Material Attachment
		glCreateTextures(GL_TEXTURE_2D, 1, &m_GBuffer.LightSpacePositionAttachment);
		glTextureStorage2D(m_GBuffer.LightSpacePositionAttachment, 1, GL_RGBA8, m_OutputWidth, m_OutputHeight);
		glTextureParameteri(m_GBuffer.LightSpacePositionAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_GBuffer.LightSpacePositionAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glNamedFramebufferTexture(m_GBuffer.Framebuffer, GL_COLOR_ATTACHMENT2, m_GBuffer.LightSpacePositionAttachment, 0);

		const GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glNamedFramebufferDrawBuffers(m_GBuffer.Framebuffer, 3, attachments);

		// GBuffer Position Attachment
		glCreateRenderbuffers(1, &m_GBuffer.DepthAttachment);
		glNamedRenderbufferStorage(m_GBuffer.DepthAttachment, GL_DEPTH_COMPONENT32F, m_OutputWidth, m_OutputHeight);
		glNamedFramebufferRenderbuffer(m_GBuffer.Framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_GBuffer.DepthAttachment);

		LOG_ASSERT(glCheckNamedFramebufferStatus(m_GBuffer.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::GBufferDepthTextureUnit, m_GBuffer.DepthAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferNormalTextureUnit, m_GBuffer.NormalAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferAlbedoTextureUnit, m_GBuffer.AlbedoAttachment);
		glBindTextureUnit(RendererBindingTable::GBufferLightSpacePositionTextureUnit, m_GBuffer.LightSpacePositionAttachment);
		glViewport(0, 0, m_OutputWidth, m_OutputHeight);

		glNamedFramebufferRenderbuffer(m_OutputBuffer.Framebuffer, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_GBuffer.DepthAttachment);
	}

	void OpenGLDeferredRenderPipeline::ConstructShadowMapBuffer(uint32_t resolution)
	{
		m_ShadowPass.Resolution = resolution;
		glCreateFramebuffers(1, &m_ShadowPass.Framebuffer);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ShadowPass.ShadowMap);
		glTextureStorage2D(m_ShadowPass.ShadowMap, 1, GL_DEPTH_COMPONENT32F, m_ShadowPass.Resolution, m_ShadowPass.Resolution);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ShadowPass.ShadowMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(m_ShadowPass.Framebuffer, GL_DEPTH_ATTACHMENT, m_ShadowPass.ShadowMap, 0);

		glNamedFramebufferDrawBuffer(m_ShadowPass.Framebuffer, GL_NONE);
		glNamedFramebufferReadBuffer(m_ShadowPass.Framebuffer, GL_NONE);

		LOG_ASSERT(glCheckNamedFramebufferStatus(m_ShadowPass.Framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer does not meet the requirements!");

		glBindTextureUnit(RendererBindingTable::DirectionalLightShadowMapTextureUnit, m_ShadowPass.ShadowMap);
	}
}