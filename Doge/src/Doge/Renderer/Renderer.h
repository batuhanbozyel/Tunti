#pragma once
#include "RenderDataManager.h"

#include "Doge/Core/Window.h"

#include "Doge/Utility/Mesh.h"
#include "Doge/Utility/Camera.h"
#include "Doge/Utility/Material.h"

namespace Doge
{
	class WindowResizeEvent;
	
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
		virtual ~Renderer() = default;
		static void Init(const WindowProps& props);

		static void Submit(const Mesh& mesh, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected = false);
		static void Submit(const std::vector<Mesh>& meshes, const Ref<MaterialInstance>& material, const glm::mat4& transform, bool isSelected = false);

		static void RenderIndexed(const Camera& camera);

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

		virtual void ResizeFramebuffer(uint32_t width, uint32_t height) = 0;
	protected:
		std::queue<RenderData> m_LightObjectsQueue;
		std::queue<RenderData> m_OutlinedObjectsQueue;
		std::unordered_map<Ref<Material>, std::queue<RenderData>> m_RenderQueue;
		std::unordered_set<uint64_t> m_ResidentTextureHandles;
	private:
		static Renderer* s_Instance;
		static RendererAPI s_RendererAPI;
	};

	static Mesh BatchMeshes(const std::vector<Mesh>& meshes);
}