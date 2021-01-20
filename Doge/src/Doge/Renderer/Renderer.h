#pragma once
#include "BufferManager.h"
#include "Doge/Utility/Light.h"

namespace Doge
{
	struct Shader;
	struct WindowProps;
	struct CubemapTexture;

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

		std::vector<Light> LightQueue;
		std::unordered_map<Ref<Material>, std::unordered_map<Ref<MaterialInstance>, std::vector<std::tuple<MeshData, glm::mat4>>>> MeshQueue;
	};

	class Renderer final
	{
	public:
		static void Init(decltype(RendererAPI::None) api);
		static void Shutdown();

		static void Submit(const std::function<void()>& renderPass);
		static void DrawMesh(const MeshData& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform);

		static decltype(RendererAPI::None) GetAPI() { return s_GraphicsAPI; }

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void SetSkybox(CubemapTexture skybox);
		static void ClearSkybox();
	private:
		static inline decltype(RendererAPI::None) s_GraphicsAPI = RendererAPI::None;
		static inline RendererAPI* s_Instance = nullptr;
	};
}