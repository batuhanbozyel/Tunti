#pragma once
#include "Light.h"
#include "Camera.h"
#include "Material.h"
#include "BufferManager.h"
#include "RendererBindingTable.h"

#include "Tunti/Events/WindowEvent.h"

namespace Tunti
{
	using SubmeshQueueElementList = std::pair<std::vector<SubmeshBuffer>, std::vector<Ref<MaterialInstance>>>;
	using MeshRenderQueueElement = std::pair<SubmeshQueueElementList, glm::mat4>;
	using MeshQueueContainer = std::unordered_map<Ref<Material>, std::unordered_map<MeshBuffer, std::vector<MeshRenderQueueElement>>>;

#pragma pack(push, 1)
	struct LightQueueContainer
	{
		_DirectionalLight DirectionalLight;
		std::array<_PointLight, RendererConstants::MaximumLightNumber> PointLights;
		uint32_t PointLightCount;
	};
#pragma pack(pop)

	class ScriptableRenderPipeline
	{
	public:
		ScriptableRenderPipeline();
		virtual ~ScriptableRenderPipeline() = default;

		void SubmitPointLight(const _PointLight& pointLight);
		void SubmitDirectionalLight(const _DirectionalLight& directionalLight);
		void SubmitEnvironmentMap(EnvironmentMapTexture environmentMap);
		void SubmitMesh(const MeshBuffer& mesh, const SubmeshQueueElementList& submeshes, const glm::mat4& transform);

		Texture2D Execute(const ShaderCameraContainer& shaderCamera);
		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
	protected:
		virtual Texture2D ExecuteImpl(const ShaderCameraContainer& shaderCamera) const = 0;
	protected:
		EnvironmentMapTexture EnvironmentMap;
		LightQueueContainer LightQueue;
		MeshQueueContainer MeshQueue;
	};
}