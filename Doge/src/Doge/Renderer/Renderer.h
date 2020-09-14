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

		static const std::unique_ptr<Framebuffer>& GetFramebuffer() { return s_Renderer->s_QuadFramebuffer; }
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
		std::unique_ptr<Framebuffer> s_QuadFramebuffer;
		std::unique_ptr<VertexArray> s_QuadVertexArray;
		std::shared_ptr<VertexBuffer> s_QuadVertexBuffer;
		std::shared_ptr<IndexBuffer> s_QuadIndexBuffer;
		std::shared_ptr<Shader> s_QuadTexturedShader;

		std::unique_ptr<Framebuffer> s_MainFramebuffer;
		std::unique_ptr<VertexArray> s_VertexArray;
		std::unique_ptr<UniformBuffer> s_ViewProjectionUniformBuffer;
		std::unique_ptr<UniformBuffer> s_LightingUniformBuffer;
		std::shared_ptr<Shader> s_ObjectOutliningShader;
		std::queue<RenderData> s_OutlineRenderQueue;
		std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> s_RenderQueue;

		std::unique_ptr<RenderData> s_PointLight;
		const Shader* s_LastShaderState = nullptr;

		static Renderer* s_Renderer;
		static RendererAPI s_API;
	};
}