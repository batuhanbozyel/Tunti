#pragma once
#include "RenderDataManager.h"

namespace Doge
{
	class WindowResizeEvent;

	class Framebuffer;
	class VertexArray;
	class UniformBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	class Model;
	class Camera;
	class Material;
	class SceneObject3D;
	class CubemapTexture;

	struct WindowProps;

	enum class RendererAPI
	{
		None = 0,
		OpenGL,
		Vulkan
	};

	class Renderer
	{
	public:
		static void Init(const WindowProps& props);

		Renderer(const WindowProps& props);
		~Renderer() = default;

		static void Submit(const RenderData& data);
		static void RenderIndexed(const Camera& camera);

		static void SetAPI(const RendererAPI& api) { s_API = api; }
		static const RendererAPI& GetAPI() { return s_API; }
		
		static void OnWindowResize(WindowResizeEvent& e);

		static const Scope<Framebuffer>& GetFramebuffer() { return s_Instance->m_QuadFramebuffer; }
		static void SetSkybox(const Ref<CubemapTexture>& skybox);
	private:
		void PrepareBufferObjects(const Camera& camera);
		void BeginRender(const Camera& camera);
		void EndRender();

		void RenderObjectsIndexed();
		void RenderOutlinedObjectsIndexed();
		void RenderLightObjectsIndexed();
		void RenderSkybox();
		void RenderFramebuffer();

		void DrawIndexed(const RenderData& renderData);

		void ConstructScreenQuadProperties(const WindowProps& props);
		void ConstructMainProperties(const WindowProps& props);
		void ConstructSkyboxProperties();
	private:
		Scope<Framebuffer> m_QuadFramebuffer;
		Scope<VertexArray> m_QuadVertexArray;
		Ref<Shader> m_QuadTexturedShader;

		Scope<Framebuffer> m_MainFramebuffer;
		Scope<VertexArray> m_MainVertexArray;
		Scope<UniformBuffer> m_ViewProjectionUniformBuffer;
		Scope<UniformBuffer> m_LightingUniformBuffer;
		Ref<Shader> m_ObjectOutliningShader;

		Scope<VertexArray> m_SkyboxVertexArray;
		Ref<Shader> m_SkyboxShader;

		std::queue<RenderData> m_OutlineRenderQueue;
		std::unordered_map<Ref<Material>, std::queue<RenderData>> m_RenderQueue;

		Ref<CubemapTexture> m_Skybox = nullptr;
		Scope<RenderData> m_PointLight;
		const Shader* m_LastShaderState = nullptr;

		static Scope<Renderer> s_Instance;
		static RendererAPI s_API;
	};
}