#pragma once
#include "RenderDataManager.h"

namespace Doge
{
	struct Mesh;
	struct WindowProps;
	struct CubemapTexture;

	class Camera;
	class MaterialInstance;
	class WindowResizeEvent;
	
	enum class RendererAPI
	{
		None = 0,
		OpenGL,
		Vulkan
	};

	class Renderer
	{
	public:
		virtual ~Renderer() = default;
		static void Init(const WindowProps& props);

		static void RenderIndexed(const Camera& camera);

		static void Submit(const Mesh& mesh, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected = false);
		static void Submit(const std::vector<Mesh>& meshes, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected = false);

		static void SetSkybox(CubemapTexture skybox);
		static void ClearSkybox();

		static void FlushRenderer();

		static void SetRendererAPI(RendererAPI api) { s_RendererAPI = api; }
		static RendererAPI GetRendererAPI() { return s_RendererAPI; }
		
		static void OnWindowResize(WindowResizeEvent& e);
	protected:
		virtual void BeginRender(const Camera& camera) = 0;
		virtual void EndRender() = 0;

		virtual void RenderObjectsIndexed() = 0;
		virtual void RenderOutlinedObjectsIndexed() = 0;
		virtual void RenderLightObjectsIndexed() = 0;
		virtual void RenderSkybox() = 0;

		virtual void FlushImpl() = 0;
		virtual void SetSkyboxImpl(CubemapTexture skybox) = 0;
		virtual void ClearSkyboxImpl() = 0;

		virtual void ResizeFramebuffer(uint32_t width, uint32_t height) = 0;
	protected:
		std::vector<RenderData> m_LightObjectsQueue;
		std::vector<RenderData> m_OutlinedObjectsQueue;
		std::unordered_map<Ref<Material>, std::vector<RenderData>> m_RenderQueue;
		std::unordered_set<uint64_t> m_ResidentTextureHandles;
	private:
		static Renderer* s_Instance;
		static RendererAPI s_RendererAPI;
	};

	static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
}