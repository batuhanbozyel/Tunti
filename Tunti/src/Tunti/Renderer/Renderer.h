#pragma once
#include "Light.h"
#include "RendererBindingTable.h"

namespace Tunti
{
	struct Shader;
	struct Texture2D;
	struct WindowProps;
	struct MeshRenderer;
	struct CubemapTexture;
	struct EnvironmentMapTexture;

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
		std::function<Texture2D()> GetFramebufferTexture;
		std::function<void(uint32_t, uint32_t)> ResizeFramebuffers;

		std::function<void(const Camera&, const glm::mat4&, const glm::vec3&)> BeginScene;
		std::vector<std::function<void()>> RenderPasses;
		std::function<void()> EndScene;

		std::function<void(CubemapTexture)> SetSkybox;
		std::function<void(EnvironmentMapTexture)> SetEnvironmentMap;

#pragma pack(push, 1)
		struct LightQueueContainer
		{
			uint32_t LightCount = 0;
			float _Padding[3] = { 0.0f };
			std::array<LightData, RendererConstants::MaximumLightNumber> Lights;
		} LightQueue;
#pragma pack(pop)
		std::unordered_map<Ref<Material>, std::unordered_map<Ref<MaterialInstance>, std::vector<std::tuple<MeshRenderer, glm::mat4>>>> MeshMultiLayerQueue;
	};

	class Renderer final
	{
	public:
		static inline decltype(RendererAPI::None) API = RendererAPI::None;
	public:
		static void Init();
		static void Shutdown();

		static void Submit(const std::function<void()>& renderPass);
		static void SubmitLight(const Light& light, const glm::vec3& position, const glm::vec3& direction);
		static void DrawMesh(const MeshRenderer& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform);

		static void ResizeFramebuffers(uint32_t width, uint32_t height);
		static Texture2D GetFramebufferTexture();
		static decltype(RendererAPI::None) GetAPI() { return API; }

		static void BeginScene(const Camera& camera, const glm::mat4& view, const glm::vec3& position);
		static void EndScene();

		static void SetSkybox(CubemapTexture skybox);
		static void SetEnvironmentMap(EnvironmentMapTexture environmentMap);
	private:
		static inline RendererAPI* s_Instance = nullptr;
	};
}