#pragma once

namespace Doge
{
	struct Mesh;
	struct Shader;
	struct WindowProps;
	struct CubemapTexture;
	struct GraphicsBuffer;

	class Camera;
	class Material;
	class Renderer;
	class MaterialInstance;
	
	class RendererAPI
	{
	public:
		friend class Renderer;
		enum {
			None = 0,
			OpenGL,
			Vulkan
		};
	protected:
		std::function<void(const Camera&)> BeginScene;
		std::vector<std::function<void()>> RenderPasses;
		std::function<void()> EndScene;

		std::function<void(CubemapTexture skybox)> SetSkybox;
		std::function<void()> ClearSkybox;

		std::function<void(uint32_t, uint32_t)> ResizeFramebuffers;

		struct RenderQueue
		{
			std::unordered_map<Ref<Material>, std::unordered_map<Ref<MaterialInstance>, std::queue<Mesh>>> MeshQueue;
		} s_RenderQueue;
	};

	class Renderer
	{
	public:
		static void Init(decltype(RendererAPI::None) api);
		static void Shutdown();

		static void Submit(const std::function<void()>& renderPass);
		static void DrawMesh(const GraphicsBuffer& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform);

		static decltype(RendererAPI::None) GetAPI() { return s_GraphicsAPI; }

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void SetSkybox(CubemapTexture skybox);
		static void ClearSkybox();

		static void Resize(uint32_t width, uint32_t height);
	private:
		static decltype(RendererAPI::None) s_GraphicsAPI;
		static RendererAPI* s_Instance;
	};
}