#pragma once
#include "Light.h"
#include "RendererBindingTable.h"

namespace Tunti
{
	struct Shader;
	struct MeshData;
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

#pragma pack(push, 1)
		struct LightQueueContainer
		{
			uint32_t LightCount = 0;
			float _Padding[3] = { 0.0f };
			std::array<LightData, RendererConstants::MaximumLightNumber> Lights;
		} LightQueue;
#pragma pack(pop)
		std::unordered_map<Ref<Material>, std::unordered_map<Ref<MaterialInstance>, std::vector<std::tuple<MeshData, glm::mat4>>>> MeshQueue;
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
		static void DrawMesh(const MeshData& mesh, const Ref<MaterialInstance>& materialInstance, const glm::mat4& transform);

		static decltype(RendererAPI::None) GetAPI() { return API; }

		static void BeginScene(const Camera& camera, const glm::mat4& view, const glm::vec3& position);
		static void EndScene();

		static void SetSkybox(CubemapTexture skybox);
		static void ClearSkybox();
	private:
		static inline RendererAPI* s_Instance = nullptr;
	};
}