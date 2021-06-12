#pragma once
#include "Tunti/Renderer/ScriptableRenderPipeline.h"

#include "OpenGLDrawParams.h"

namespace Tunti
{
	class OpenGLShaderProgram;

	struct GeometryBuffer
	{
		GLuint Framebuffer = 0;
		GLuint NormalAttachment = 0;
		GLuint AlbedoAttachment = 0;
		GLuint LightSpacePositionAttachment = 0;
		GLuint DepthAttachment = 0;
	};

	struct ShadowPassBuffer
	{
		GLuint Framebuffer = 0;
		GLuint ShadowMap = 0;
		uint32_t Resolution = 1024;
	};

	struct OutputBuffer
	{
		GLuint Framebuffer = 0;
		GLuint ColorAttachment = 0;
	};

	class OpenGLDeferredRenderPipeline : public ScriptableRenderPipeline
	{
	public:
		OpenGLDeferredRenderPipeline();
		~OpenGLDeferredRenderPipeline();

		virtual Texture2D GetDebugOutputTexture(uint32_t idx = 0) const override;
		virtual void OnWindowResize(uint32_t width, uint32_t height) override;
	private:
		virtual Texture2D ExecuteImpl(const ShaderCameraContainer& shaderCamera) const override;

		void UpdateBuffers(const ShaderCameraContainer& shaderCamera) const;
		void ShadowPass() const;
		void GeometryPass() const;
		void LightingPass() const;
		void SkyboxPass() const;

		void DrawSceneGeometry(const Ref<OpenGLShaderProgram>& shader) const;
		void DrawSceneGeometryWithMaterials(const Ref<OpenGLShaderProgram>& shader) const;

		void ConstructOutputBuffer();
		void ConstructGBuffer();
		void ConstructShadowMapBuffer(uint32_t resolution);
	private:
		GLuint m_CameraUniformBuffer;
		GLuint m_LightsUniformBuffer;

		GeometryBuffer m_GBuffer;
		ShadowPassBuffer m_ShadowPass;
		OutputBuffer m_OutputBuffer;

		Ref<OpenGLShaderProgram> m_ShadowPassShader;
		Ref<OpenGLShaderProgram> m_GeometryPassShader;
		Ref<OpenGLShaderProgram> m_PBRLightingPassShader;
		Ref<OpenGLShaderProgram> m_SkyboxPassShader;

		uint32_t m_OutputWidth = 1280, m_OutputHeight = 720;
	};
}