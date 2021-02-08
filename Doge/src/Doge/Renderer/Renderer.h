#pragma once

namespace Doge
{
	struct Shader;
	struct MeshData;
	struct LightData;
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
		std::function<void(const Camera&, const glm::mat4&, const glm::vec3&)> BeginScene;
		std::vector<std::function<void()>> RenderPasses;
		std::function<void()> EndScene;

		std::function<void(CubemapTexture skybox)> SetSkybox;
		std::function<void()> ClearSkybox;

		std::vector<LightData> LightQueue;
		std::unordered_map<Ref<Material>, std::unordered_map<Ref<MaterialInstance>, std::vector<std::tuple<MeshData, glm::mat4>>>> MeshQueue;
	};

	class Renderer final
	{
	public:
		static void Init(decltype(RendererAPI::None) api);
		static void Shutdown();

		static void Submit(const std::function<void()>& renderPass);
		static void DrawLight(const LightData& light, const glm::mat4& transform);
		static void DrawMesh(const MeshData& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform);

		static decltype(RendererAPI::None) GetAPI() { return s_GraphicsAPI; }

		static void BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& position);
		static void EndScene();

		static void SetSkybox(CubemapTexture skybox);
		static void ClearSkybox();
	private:
		static inline decltype(RendererAPI::None) s_GraphicsAPI = RendererAPI::None;
		static inline RendererAPI* s_Instance = nullptr;
	};
}