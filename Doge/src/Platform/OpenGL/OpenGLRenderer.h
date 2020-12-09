#pragma once
#include "Doge/Renderer/Renderer.h"

namespace Doge
{
	class OpenGLShader;
	class OpenGLFramebuffer;
	class OpenGLVertexArray;
	class OpenGLIndexBuffer;
	class OpenGLVertexBuffer;
	class OpenGLUniformBuffer;
	class OpenGLCubemapTexture;

	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(const WindowProps& props);
		~OpenGLRenderer() = default;
	private:
		virtual void BeginRender(const Camera& camera) override;
		virtual void EndRender() override;

		virtual void RenderObjectsIndexed() override;
		virtual void RenderOutlinedObjectsIndexed() override;
		virtual void RenderLightObjectsIndexed() override;
		virtual void RenderSkybox() override;

		virtual void FlushImpl() override;
		virtual void SetSkyboxImpl(CubemapTexture skybox) override;
		virtual void ClearSkyboxImpl() override;

		virtual void ResizeFramebuffer(uint32_t width, uint32_t height) override;
	private:
		void ConstructScreenQuadProperties(const WindowProps& props);
		void ConstructMainProperties(const WindowProps& props);
		void ConstructSkyboxProperties();

		void SetCommonUniformProperties(const Ref<Material>& material) const;
		void SetUniqueUniformProperties(const Ref<MaterialInstance>& materialInstance) const;

		void PrepareBufferObjects(const Camera& camera);
		void DrawIndexed(const std::vector<OpenGLVertexBuffer>& VBOs, const OpenGLIndexBuffer& IBO);
	private:
		// Screen Rendering data
		Scope<OpenGLFramebuffer> m_QuadFramebuffer;
		Scope<OpenGLVertexArray> m_QuadVertexArray;
		Scope<OpenGLShader> m_QuadTexturedShader;

		// Offscreen Rendering data
		Scope<OpenGLFramebuffer> m_MainFramebuffer;
		Scope<OpenGLVertexArray> m_MainVertexArray;
		Scope<OpenGLUniformBuffer> m_ViewProjectionUniformBuffer;
		Scope<OpenGLUniformBuffer> m_LightingUniformBuffer;
		Scope<OpenGLShader> m_ObjectOutliningShader;

		// Skybox Rendering data
		Scope<OpenGLVertexArray> m_SkyboxVertexArray;
		Scope<OpenGLShader> m_SkyboxShader;

		const OpenGLShader* m_LastShaderState = nullptr;
	};
}