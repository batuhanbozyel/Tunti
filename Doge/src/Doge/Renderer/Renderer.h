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
		static void Init(const RendererAPI& api, const WindowProps& props);

		static void Submit(const RenderData& data);
		static void RenderIndexed(const Camera& camera);

		static const RendererAPI& GetAPI() { return s_API; }
	private:
		static void PrepareBufferObjects(const Camera& camera);
		static void RenderObjectIndexed(const RenderData& renderData);
	private:
		static std::shared_ptr<Framebuffer> s_Framebuffer;
		static std::shared_ptr<VertexArray> s_VertexArray;
		static std::shared_ptr<UniformBuffer> s_ViewProjectionUniformBuffer;
		static std::shared_ptr<UniformBuffer> s_LightingUniformBuffer;
		static std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> s_RenderQueue;

		static const Shader* s_LastShaderState;
		static RendererAPI s_API;
	};
}