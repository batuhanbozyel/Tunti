#pragma once
#include "RenderDataManager.h"

namespace Doge
{
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

		static const std::unique_ptr<Framebuffer>& GetFramebuffer() { return s_QuadFramebuffer; }
	private:
		static void PrepareBufferObjects(const Camera& camera);
		static void RenderObjectIndexed(const RenderData& renderData);
	private:
		static std::unique_ptr<Framebuffer> s_QuadFramebuffer;
		static std::unique_ptr<VertexArray> s_QuadVertexArray;
		static std::shared_ptr<VertexBuffer> s_QuadVertexBuffer;
		static std::shared_ptr<IndexBuffer> s_QuadIndexBuffer;
		static std::shared_ptr<Shader> s_QuadTexturedShader;

		static std::unique_ptr<VertexArray> s_VertexArray;
		static std::unique_ptr<UniformBuffer> s_ViewProjectionUniformBuffer;
		static std::unique_ptr<UniformBuffer> s_LightingUniformBuffer;
		static std::shared_ptr<Shader> s_ObjectOutliningShader;
		static std::queue<RenderData> s_OutlineRenderQueue;
		static std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> s_RenderQueue;

		static const Shader* s_LastShaderState;
		static RendererAPI s_API;
	};
}