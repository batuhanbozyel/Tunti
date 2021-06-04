#pragma once
#include "Light.h"
#include "RendererBindingTable.h"

namespace Tunti
{
	struct Shader;
	struct Material;
	struct Texture2D;
	struct MeshBuffer;
	struct WindowProps;
	struct SubmeshBuffer;
	struct CubemapTexture;
	struct MaterialInstance;
	struct EnvironmentMapTexture;

	class Camera;
	class Renderer;

	using SubmeshQueueElementList = std::pair<std::vector<SubmeshBuffer>, std::vector<Ref<MaterialInstance>>>;
	using MeshRenderQueueElement = std::pair<SubmeshQueueElementList, glm::mat4>;

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

		std::function<void(EnvironmentMapTexture)> SetEnvironmentMap;

#pragma pack(push, 1)
		struct LightQueueContainer
		{
			_DirectionalLight DirectionalLight;
			std::array<_PointLight, RendererConstants::MaximumLightNumber> PointLights;
			uint32_t PointLightCount;
		} LightQueue;
#pragma pack(pop)
		std::unordered_map<Ref<Material>, std::unordered_map<MeshBuffer, std::vector<MeshRenderQueueElement>>> MeshQueue;
	};

	class Renderer final
	{
	public:
		static inline decltype(RendererAPI::None) API = RendererAPI::None;
	public:
		static void Init();
		static void Shutdown();

		static void Submit(const std::function<void()>& renderPass);
		static void SubmitPointLight(const _PointLight& pointLight);
		static void SubmitDirectionalLight(const _DirectionalLight& directionalLight);
		static void SubmitEnvironmentMap(EnvironmentMapTexture environmentMap);
		static void DrawMesh(const MeshBuffer& mesh, const SubmeshQueueElementList& submeshes, const glm::mat4& transform);

		static void ResizeFramebuffers(uint32_t width, uint32_t height);
		static Texture2D GetFramebufferTexture();
		static decltype(RendererAPI::None) GetAPI() { return API; }

		static void DrawScene(const Camera& camera, const glm::mat4& view, const glm::vec3& position);
	private:
		static inline RendererAPI* s_Instance = nullptr;
	};
}