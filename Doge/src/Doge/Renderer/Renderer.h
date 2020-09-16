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

	class Camera;
	class Material;
	class SceneObject3D;
	class Model;

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

		static void Submit(const RenderData& data);
		static void RenderIndexed(const Camera& camera);

		static void SetAPI(const RendererAPI& api) { s_API = api; }
		static const RendererAPI& GetAPI() { return s_API; }

		static void OnWindowResize(WindowResizeEvent& e);

		static const Scope<Framebuffer>& GetFramebuffer() { return s_Renderer->s_QuadFramebuffer; }
	private:
		Renderer(const WindowProps& props);
		~Renderer();

		void PrepareBufferObjects(const Camera& camera);
		void BeginRender(const Camera& camera);
		void EndRender();

		void RenderObjectsIndexed();
		void RenderOutlinedObjectsIndexed();
		void RenderLightObjectsIndexed();
		void RenderFramebuffer();

		void DrawIndexed(const RenderData& renderData);
	private:
		Scope<Framebuffer> s_QuadFramebuffer;
		Scope<VertexArray> s_QuadVertexArray;
		Scope<VertexBuffer> s_QuadVertexBuffer;
		Scope<IndexBuffer> s_QuadIndexBuffer;
		Ref<Shader> s_QuadTexturedShader;

		Scope<Framebuffer> s_MainFramebuffer;
		Scope<VertexArray> s_VertexArray;
		Scope<UniformBuffer> s_ViewProjectionUniformBuffer;
		Scope<UniformBuffer> s_LightingUniformBuffer;
		Ref<Shader> s_ObjectOutliningShader;
		std::queue<RenderData> s_OutlineRenderQueue;
		std::unordered_map<Ref<Material>, std::queue<RenderData>> s_RenderQueue;

		Scope<RenderData> s_PointLight;
		const Shader* s_LastShaderState = nullptr;

		static Renderer* s_Renderer;
		static RendererAPI s_API;
	};
}