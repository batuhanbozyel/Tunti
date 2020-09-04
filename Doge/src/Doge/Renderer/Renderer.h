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

	class Renderer
	{
	public:
		static void Init(const WindowProps& props);

		static void Submit(const RenderData& data);
		static void DrawIndexed(const Camera& camera);
	private:
		static void PrepareBufferObjects(const Camera& camera);
		static void RenderObjectIndexed(const RenderData& renderData);
	private:
		static std::unique_ptr<Framebuffer> s_Framebuffer;
		static std::unique_ptr<VertexArray> s_VertexArray;
		static std::unique_ptr<UniformBuffer> s_ViewProjectionUniformBuffer;
		static std::unique_ptr<UniformBuffer> s_LightingUniformBuffer;
		static std::unordered_map<std::shared_ptr<Material>, std::queue<RenderData>> s_RenderQueue;

		static const Shader* s_LastShaderState;
	};
}